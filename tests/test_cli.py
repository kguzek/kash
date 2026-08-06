from tests.utils.program import PROGRAM_NAME, run_program, run_result_type


def assert_version_output(result: run_result_type) -> None:
    assert result.stdout.startswith(PROGRAM_NAME)
    assert "Copyright (c) 2026 Konrad Guzek" in result.stdout


def assert_help_output(result: run_result_type) -> None:
    assert result.stdout.startswith(f"Usage: {PROGRAM_NAME} [option]")


def test_cli_version_happy():
    assert_version_output(run_program("--version"))


def test_cli_version_with_additional_args():
    assert_version_output(run_program("--version", "additional-argument"))


def test_cli_help_happy():
    assert_help_output(run_program("--help"))


def test_cli_help_with_additional_args():
    assert_help_output(run_program("--help", "additional-argument"))


def test_cli_repl_happy():
    result = run_program(input="echo 'hello world'\nexit")
    assert result.stdout == "hello world\n"
