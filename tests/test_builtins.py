from utils.program import run_command


def test_echo():
    result = run_command("echo 'hello world'")
    assert result.stdout == "hello world\n"
