from tests.utils.program import run_command


def assert_success(input: str, output: str) -> None:
    result = run_command(input)
    assert result.stdout == f"{output}\n"
    assert result.returncode == 0
    assert result.stderr == ""


def assert_failure(input: str, output: str, code: int | None = None) -> None:
    result = run_command(input)
    assert result.stderr == f"{output}\n"
    if code == None:
        assert result.returncode != 0
    else:
        assert result.returncode == code
    assert result.stdout == ""
