# Beebasm Todo List

## File Maintenance Rules
- Use todo_write at start of EVERY session
- ALWAYS send everything in todo.md to todo_write at session start to keep them in sync
- Read task file at start of session
- Update task file after each step
- Update transcript file with each prompt/response
- ALWAYS update TODO.md when todo changes

## Project Status
- FUNCTION feature: WORKING (debug output removed)
- All tests: PASS
- Parse-time RETURN detection: COMPLETE
- @ modifier: WORKING (write-only, cannot read back)
- LET: Working (needs * prefix inside FOR loops to read outside)
- Summary: UPDATED
- Multiple ^ prefix: WORKING
- Scope prefixes (*): WORKING
- Scope prefixes (^): Needs more testing

## Project Info
- Project: Beebasm - BBC Micro assembler
- Source directory: /media/storage/projects/beebasm/src
- Test directory: /media/storage/projects/beebasm/test
- Compile: cd src && make code (clean: make clean && make code)
- Assemble: cd ~/Desktop/projects/wip/beebasm-tests && ./beebasm -v -i filename.6502

## Tools
- Use task agent to check transcript on error: Launch task agent with prompt to check /media/storage/projects/beebasm/transcript for incomplete commands, have it write findings to /tmp/transcript_check.txt, then read the file
- Error handler must detect shell command failures (Exit Code != 0)

## Verification Results (Feb 26, 2026)
- MULTIPASS: Working
- XORG: Working
- @ modifier: Working (write-only, cannot read back)
- LABEL: Working
- LET: Working (needs * prefix inside FOR loops)
- FUNCTION: Working (debug output removed)
- Parse-time RETURN detection: Working
- Scope prefixes (*): Working
- Scope prefixes (^): Needs more testing

## Summary Inaccuracies Found
- @ modifier summary says "acts as mutable variables" but doesn't mention it's write-only
- LET summary says "optional scope prefix" but inside FOR loops needs * to be readable outside
- FUNCTION: Fixed - debug output removed

Last updated: February 26, 2026
