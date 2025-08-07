---
applyTo: '**'
---
Provide project context and coding guidelines that AI should follow when generating code, answering questions, or reviewing changes.

## Debug Interaction Best Practice


For best results when interacting with agent mode, always run debug builds and unit tests using the command line. The agent must always validate the current working directory before issuing any command, to ensure all commands are executed in the correct context and paths resolve as expected. Prefer running commands from the project root directory whenever possible. This ensures you get full console output and better visibility for agent-assisted debugging and test validation.

**Recommended commands:**

```bash
# For main application debug
make debug && ./bin/x86_64-native/debug/hello_world

# For unit tests (run after every code change)
cd tests/<module> && make clean && make && ./bin/<module>Test
```

This approach allows the agent to capture and analyze all console and test output, making troubleshooting, code review, and correctness validation more effective.

**Debugging & Testing Workflow:**

- After every code change, always build and run the relevant unit tests in the command line to confirm correctness.
- If you encounter issues, always try to debug or fix up to 5 times, making adjustments as needed after each attempt.
- After each run, review the console and test output and error messages to guide your next step.
- If the problem persists after 5 attempts, escalate or seek further help with detailed logs and context.