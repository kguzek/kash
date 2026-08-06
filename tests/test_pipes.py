from tests.utils.assertions import assert_success


def test_pipe_input_handling():
    assert_success(
        "ls --version | grep 'free software'",
        "This is free software: you are free to change and redistribute it.",
    )


def test_pipe_optional_spacing():
    for spacing in (
        "echo test | cat -n",
        "echo test |cat -n",
        "echo test| cat -n",
        "echo test|cat -n",
    ):
        assert_success(spacing, "     1\ttest")


def test_pipe_stacking():
    assert_success(
        "ls --version | grep GPL | tr -d ':/ '",
        "LicenseGPLv3+GNUGPLversion3orlater<httpsgnu.orglicensesgpl.html>.",
    )
