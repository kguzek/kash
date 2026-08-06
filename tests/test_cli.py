from utils.program import PROGRAM_NAME, run_program


def assert_version_output(stdout: str) -> None:
    assert stdout.startswith(PROGRAM_NAME)
    assert "Copyright (c) 2026 Konrad Guzek" in stdout


def assert_help_output(stdout: str) -> None:
    assert stdout.startswith(f"Usage: {PROGRAM_NAME} [option]")


def test_cli_version_happy():
    assert_version_output(run_program("--version").stdout)


def test_cli_version_with_additional_args():
    assert_version_output(run_program("--version", "additional-argument").stdout)


def test_cli_help_happy():
    assert_help_output(run_program("--help").stdout)


def test_cli_help_with_additional_args():
    assert_help_output(run_program("--help", "additional-argument").stdout)
