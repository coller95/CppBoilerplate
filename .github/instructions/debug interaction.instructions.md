---
applyTo: '**'
---

Provide project context and coding guidelines that AI should follow when generating code, answering questions, or reviewing changes.

**Note:** Whenever instructions refer to actions on "the whole repo," always exclude any files or directories listed in `.gitignore`. Ignored files and folders must not be included in summaries, code changes, or batch operations.

## Efficient Debug & Test Workflow

To maximize productivity and feedback speed, always:

1. **Validate Directory:**
   - Always check and set the working directory to the project root before running any build or test commands.

2. **Batch Build & Test:**
   - Use batch commands to build and test in one step, minimizing context switches and manual intervention.

3. **Recommended Commands:**

   ```bash
   # Build and run main application (from project root)
   make debug && make run_debug

   # Build and run all unit tests (from project root)
   make test
   ```

   - if really needed, you can use the following commands to clean and build:
   ```bash
   # Clean and Build and run main application (from project root)
   make clean &&make debug && make run_debug

   # Clean and Build and run all unit tests (from project root)
   make test-clean && make test
   ```


4. **After Every Change:**
   - Always run the relevant unit tests after any code change to catch issues early.

5. **Iterative Debugging:**
   - If a test fails, fix the code and immediately rerun the test. Repeat up to 5 times if needed.
   - Review all output and error messages to guide your next step.

6. **Escalate if Needed:**
   - If the problem persists after 5 attempts, escalate with detailed logs and context.

**Summary:**
- Always work from the project root for consistency.
- Use batch scripts for rapid feedback across all modules.
- Run tests after every change for continuous validation.