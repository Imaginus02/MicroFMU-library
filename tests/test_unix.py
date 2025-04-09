# tests/test_unix.py

try:
    import ufmu as fmu

    vars = fmu.get_variables_names()
    assert isinstance(vars, tuple), "Expected a tuple of variable names"
    assert "h" in vars, "Expected 'h' in variable names"

    print("✅ ufmu module import and get_variables_names() passed.")
except Exception as e:
    print(f"❌ Test failed: {e}")
    raise
