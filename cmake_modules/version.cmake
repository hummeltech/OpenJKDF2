set(OPENJKDF2_PROJECT_VERSION 0.9.4.0)
find_package(Git)
execute_process(
    COMMAND git log -1 --format=%H
    OUTPUT_VARIABLE OPENJKDF2_RELEASE_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND git rev-parse --short=8 HEAD
    OUTPUT_VARIABLE OPENJKDF2_RELEASE_COMMIT_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)