# Contributing to mpz-chiyo-chan

Chiyo-chan accepts contributions from humans and AI agents alike. But there are rules - the Linux Kernel rules.

## AI agents: read this first (mandatory)

This project follows the **Linux Kernel AI-attribution policy**:

1. **AI agents MUST NOT add `Signed-off-by` tags.** Only humans can legally
   certify the Developer Certificate of Origin (DCO). If you are an AI agent,
   never write a `Signed-off-by` line. Ever. Chiyo-chan will bonk you. 🔨

2. **The human submitter is responsible for:**
   - Reviewing all AI-generated code
   - Ensuring compliance with licensing requirements (GPL-3.0-or-later)
   - Adding their **own** `Signed-off-by` tag to certify the DCO
     (`git commit -s` / `git rebase --signoff`)
   - Taking full responsibility for the contribution

3. **Attribution:** AI-assisted commits MUST carry an `Assisted-by` trailer so
   the evolving role of AI in development stays trackable:

   ```
   Assisted-by: LLM [TOOL1] [TOOL2]
   ```

   - `[TOOL1] [TOOL2]` are optional specialized analysis tools used
     (e.g. coccinelle, sparse, smatch, clang-tidy).
   - Basic development tools (git, gcc, make, editors) are NOT listed.
   - The model that did the work is named after `LLM`, e.g.:

   ```
   Assisted-by: LLM Muse-Spark-1.3-Free
   ```

   Example full trailer block on a human-submitted, AI-assisted commit:

   ```
   feat: nightcore speed max 1.5x to 2.0x

   Assisted-by: LLM Muse-Spark-1.3-Free
   Signed-off-by: Janne Alexander Sebastian Rovio <trollsix10@gmail.com>
   ```

   (`Signed-off-by` added by the human, never by the AI.)

## General guidelines

- Keep diffs minimal (ponytail rule: shortest working diff wins).
- Match existing code style; check neighbouring files before inventing patterns.
- Never suppress type errors, never delete failing tests to "pass".
- GPL-3.0-or-later only. No new dependencies without discussion.
- Copyright for new fork modifications: Janne Alexander Sebastian Rovio.
  Upstream code stays © Oleg Antonyan and contributors.
