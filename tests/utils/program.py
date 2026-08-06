import subprocess

PROGRAM_NAME = "kash"
PROGRAM_EXECUTABLE_PATH = f"./build/{PROGRAM_NAME}"


def run_program(*args: str) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        [PROGRAM_EXECUTABLE_PATH, *args],
        check=False,
        text=True,
        capture_output=True,
    )
    return result


def run_command(command: str) -> subprocess.CompletedProcess[str]:
    return run_program("-c", command)
