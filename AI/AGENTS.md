# SOAR Firmware: AI Agent Instructions

> **DRAFT.** This file is the shared source of truth for AI coding agents (Codex,
> Claude Code, Gemini CLI, etc.) working in SOAR avionics repositories. It lives in
> `SoarOS/AI/AGENTS.md` and is copied into each board repo's root `AGENTS.md`.
> Edit it here, not in board repos.
>
> Rules marked **(proposed)** are not yet confirmed by the software leads. Rules
> without that mark come directly from SoarOS source code or its comments.

These repositories are STM32 firmware built in STM32CubeIDE on FreeRTOS, using
SoarOS (Cube++) C++ wrappers. Code runs on flight hardware. Prefer small, reviewable
changes, and ask when unsure rather than guessing.

## 1. Hard restrictions: never do these

1. **Never edit CubeMX-generated code outside `USER CODE BEGIN` / `USER CODE END`
   blocks.** This covers `Core/`, `Drivers/` (STM32 HAL/CMSIS), `Middlewares/` and
   `FATFS/`. CubeMX overwrites anything outside those blocks on the next codegen.
2. **Never edit the `.ioc` file, linker scripts (`*.ld`), `.cproject`, `.project`,
   `.mxproject` or `*.launch` files.** Pin, clock and peripheral changes are made by
   a human in CubeMX. Explain what needs to change instead.
3. **Never edit submodule contents from a board repo.** This includes `SoarOS/`,
   `PeripheralDriversSubmodule/` / `SoarDrivers/` / `SoarPeripherals/`,
   `SoarCommunications/`, `SoarProtocols/` and any other submodule. Changes to shared
   code go through a PR to that submodule's own repository.
4. **Never edit third-party libraries**, e.g.
   `SoarOS/Libraries/embedded-template-library/`.
5. **Never invent hardware details.** Pins, peripheral instances (`hspi2`,
   `UART8`, ...), sensor registers and timings come from the `.ioc`, `BOARD.md`
   or the datasheet. If you don't have them, ask.
6. **Never commit build output** (`Debug/`, `Release/`, `*.elf`, `*.bin`, `*.map`).
7. **Never flash hardware**, run CubeMX code generation, or push to a
   remote unless the user explicitly asks.

## 2. Where code goes

- Application code lives in `Components/` (some boards also use `Application/`).
  Do not put application logic in `Core/Src/main.c`.
- `main.c` only calls `run_interface()` inside `USER CODE BEGIN 2`. Control then
  passes to `run_main()` in `main_avionics.cpp`, which initializes tasks and calls
  `osKernelStart()`.
- C-to-C++ bridges (IRQ handlers calling C++ drivers) go in
  `Components/Core/RunInterface.cpp` inside `extern "C"`.
- System-wide constants (task priorities, queue depths, stack sizes, UART aliases)
  go in `Components/SystemDefines.hpp`.

## 3. Tasks

Every task follows the SoarOS task pattern (see
`Components/Examples/Tasks/TaskTemplate.*` in the template repos):

- It is a **singleton** deriving from `Task`: `static X& Inst()`, a private
  constructor, and copy-construction and assignment disabled.
- The constructor passes the queue depth to `Task(DEPTH)`.
- `InitTask()` asserts `rtTaskHandle == nullptr`, calls `xTaskCreate(...)` using
  the priority and stack constants from `SystemDefines.hpp`, then asserts
  `rtValue == pdPASS`.
- Each task's `PRIORITY`, `QUEUE_DEPTH` and `STACK_DEPTH_WORDS` are `constexpr`
  values in `SystemDefines.hpp`, each with a comment.
- `InitTask()` is called from `run_main()` **before** `osKernelStart()`.
- `Run()` is a `while (1)` loop that blocks on `qEvtQueue->ReceiveWait(cm)` (or a
  timed `Receive`) and dispatches to `HandleCommand(cm)`.
- `HandleCommand` switches on `cm.GetCommand()`, then on `cm.GetTaskCommand()`,
  and has a `default:` branch that prints with `SOAR_PRINT`.

## 4. Commands and inter-task communication

- Tasks communicate by sending `Command` objects to each other's event queues
  (`Task::SendCommand`, `Queue::Send`). Do not share mutable globals between tasks
  without a `Mutex`.
- **Every `Command` taken out of a queue must have `cm.Reset()` called on it, on
  every code path, including `default:` and early returns.** `Reset()` frees
  command-owned data, and missing it leaks heap until the allocation counter
  asserts (`MAX_NUMBER_OF_COMMAND_ALLOCATIONS`).
- Attach data with `AllocateData()` / `CopyDataToCommand()`, which the receiver
  frees via `Reset()`. Use `SetCommandToStaticExternalBuffer()` only for static
  buffers, and synchronize access to them.
- After a successful `Send`, the sender must not `Reset()` whether it succeeds in sending.
  the command. Ownership of its data passes to the receiver.
- Use `DataBroker::Publish<T>` / `Subscribe<T>(this)` for sensor and telemetry
  data shared by several tasks. On receipt, use `DataBroker::ExtractData<T>(cm)`,
  then `cm.Reset()`.

## 5. Interrupts (ISRs)

- In an ISR, use only the `FromISR` variants: `Queue::SendFromISR`,
  `Mutex::LockFromISR` / `UnlockFromISR`.
- Keep ISRs short: capture the data, signal a task, return. Follow the
  `DebugTask::InterruptRxData` pattern.
- Never call `SOAR_PRINT`, `SOAR_ASSERT`, `osDelay`, blocking queue sends
  (`Queue::Send`), blocking mutex locks (`Mutex::Lock`) or anything that
  allocates from an ISR. `SOAR_PRINT` (`cube_print`) takes a blocking mutex,
  allocates heap memory and does a blocking queue send, which can hard-fault or
  deadlock when called from an interrupt.

## 6. Output, errors and timing

- Print only with `SOAR_PRINT(...)`. Never use `printf` / `puts` directly.
  `SOAR_PRINT` routes through the UART task.
- Check possible program errors (allocation results, RTOS return values, invalid
  states) with `SOAR_ASSERT(expr, "message %d", ...)`.
- Convert time with `MS_TO_TICKS()` / `TICKS_TO_MS()`. Never hard-code tick
  counts.
- Inside tasks, wait with `osDelay()` / `vTaskDelay()`, never
  `HAL_Delay()`, which busy-waits and starves lower-priority tasks.
- `Timer` callbacks you provide must call `Timer::DefaultCallback(xTimer)` so the
  timer's state stays correct.

## 7. Memory

- Allocate only through `soar_malloc` / `soar_free`, or `new` / `delete`, which
  SoarOS overrides to use the FreeRTOS heap. Never call `malloc` / `free`
  directly.
- After `osKernelStart()`, every `new` / `delete` must be closely justified.
  Prefer static allocation, or allocating during `InitTask()`.
- Do not use dynamically growing STL containers (`std::vector`,
  `std::string`, `std::map`, ...) in task code. Prefer fixed-size arrays or ETL
  containers (`etl::vector<T, N>`, etc.). This is not a hard restriction, but it
  is risky on memory-limited hardware.

## 8. Style

- C++ is formatted with clang-format: Google style, `ColumnLimit: 120`,
  `SortIncludes: false` (see `.pre-commit-config.yaml`).
- New files use the SOAR header and section-comment layout from
  `Components/Examples/FileTemplates/` (`@file`, `@author`, `@date`, `@brief`,
  then `INCLUDES`, `MACROS AND DEFINES`, `TYPEDEFS`, `CLASS DEFINITIONS`, ...).
- Constants: `constexpr`, ALL_CAPS, each with a comment explaining its use.
  Prefer `constexpr` over `#define` unless it's a string or a readable
  expression.
- Document functions with `@brief` / `@param` / `@return` Doxygen comments.

## 9. Working style for agents

- Only write or edit code when the user explicitly asks for it (e.g.
  "implement", "fix", "add", "write"). For questions, explain and suggest
  instead. This repo is used by university students, so prefer explaining _why_
  over handing over finished code.
- Read the existing task or driver you are changing, and match its patterns,
  before writing new code.
- After making changes, summarize what you changed and why, and ask the user to
  review it before committing.
- Keep changes scoped to the request. Don't refactor unrelated code.
- If a change needs `.ioc`, submodule or hardware changes, stop and explain what a
  human needs to do.
- State clearly what you could not verify. You cannot build or flash, so say that
  the code is untested on hardware.
