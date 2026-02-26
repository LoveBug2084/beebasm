# Beebasm Todo List

## File Maintenance Rules
- Read task file at start of session
- Update task file after each step
- Update transcript file with each prompt/response
- Use todo_write at start of session
- ALWAYS update TODO.md when todo changes

## Project Status
- FUNCTION feature is WORKING
- All tests PASS
- Parse-time RETURN detection COMPLETE
- @ modifier: WORKS but CANNOT be READ (write only)
- LET: Works but needs * prefix inside FOR loops to read outside
- Summary UPDATED
- Multiple ^ prefix NOW WORKING
- NEVER use git commands (absolute rule)

## Project Info
- Project: Beebasm - BBC Micro assembler
- Source directory: /media/storage/projects/beebasm/src
- Test directory: /media/storage/projects/beebasm/test
- Compile: cd src && make code (clean: make clean && make code)
- Assemble: cd beebasm dir && ./beebasm -v -i filename.6502

## Tools
- Use task agent to check transcript on error: Launch task agent with prompt to check /media/storage/projects/beebasm/transcript for incomplete commands, have it write findings to /tmp/transcript_check.txt, then read the file
- Error handler must detect shell command failures (Exit Code != 0)

## Verification Results (Feb 25, 2026)
- MULTIPASS: Working
- XORG: Working
- @ modifier: Working (write-only, cannot read back)
- LABEL: Working
- LET: Working (needs * prefix inside FOR loops)
- FUNCTION: Working (but has DEBUG output)
- Parse-time RETURN detection: Working
- Scope prefixes (*): Working
- Scope prefixes (^): Needs more testing

## Summary Inaccuracies Found
- @ modifier summary says "acts as mutable variables" but doesn't mention it's write-only
- LET summary says "optional scope prefix" but inside FOR loops needs * to be readable outside
- FUNCTION has debug output not mentioned in summary

Last updated: February 25, 2026
