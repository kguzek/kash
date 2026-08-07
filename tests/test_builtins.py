from tests.utils.assertions import assert_failure, assert_success

BUILTINS = [
    "exit",
    "echo",
    "type",
    "pwd",
    "cd",
    "complete",
    "jobs",
    "history",
    "declare",
]


def test_echo():
    assert_success("echo", "")
    assert_success("  echo  ", "")
    assert_success("echo hello world", "hello world")
    assert_success("echo hello world", "hello world")
    assert_success("echo 'hello world'", "hello world")
    assert_success("echo 'hello' world", "hello world")
    assert_success("echo 'hello   world'", "hello   world")
    assert_success("echo 'foo'bar\"baz\"", "foobarbaz")


def test_type_builtin():
    for builtin in BUILTINS:
        assert_success(f"type {builtin}", f"{builtin} is a shell builtin")


def test_type_not_found():
    command_name = "__KASH_TEST_INVALID_COMMAND_NOT_FOUND_FAKE_NAME"
    assert_failure(f"type {command_name}", f"{command_name}: not found")


def test_type_external():
    command_name = "which"
    assert_success(f"type {command_name}", f"{command_name} is /usr/bin/{command_name}")
