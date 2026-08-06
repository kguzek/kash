import subprocess

PROGRAM_NAME = "kash"
PROGRAM_EXECUTABLE_PATH = f"./build/{PROGRAM_NAME}"


run_result_type = subprocess.CompletedProcess[str]


def run_program(*args: str, input: str | None = None) -> run_result_type:
    result = subprocess.run(
        [PROGRAM_EXECUTABLE_PATH, *args],
        check=False,
        text=True,
        capture_output=True,
        input=input,
    )
    return result


def run_command(command: str) -> run_result_type:
    return run_program("-c", command)
