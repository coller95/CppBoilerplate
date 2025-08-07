---
applyTo: '**'
---
Provide project context and coding guidelines that AI should follow when generating code, answering questions, or reviewing changes.

## Debug Interaction Best Practice

For best results when interacting with agent mode, always run debug builds using the command line. This ensures you get full console output and better visibility for agent-assisted debugging.

**Recommended command:**

```bash
make debug && ./bin/x86_64-native/debug/hello_world
```

This approach allows the agent to capture and analyze all console output, making troubleshooting and code review more effective.

**Debugging Workflow:**

- If you encounter issues, always try to debug up to 5 times, making adjustments as needed after each attempt.
- After each run, review the console output and error messages to guide your next step.
- If the problem persists after 5 attempts, escalate or seek further help with detailed logs and context.