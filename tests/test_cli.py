import pathlib
import subprocess
import tempfile
import unittest
import os

ROOT = pathlib.Path(__file__).resolve().parents[1]
SUFFIX = ".exe" if os.name == "nt" else ""
EXECUTABLE = ROOT / ("tle_calc"+SUFFIX)
if not EXECUTABLE.exists():
    EXECUTABLE = ROOT / "build" / ("tle_calc"+SUFFIX)

def invoke(*arguments, text=None):
    return subprocess.run([str(EXECUTABLE), *map(str,arguments)], input=text, text=True, capture_output=True)

class CliTests(unittest.TestCase):
    def test_help(self):
        result = invoke("--help")
        self.assertEqual(result.returncode, 0)
        self.assertIn("usage:", result.stdout)

    def test_fixture_period(self):
        result = invoke(ROOT / "sample_leo.txt")
        self.assertEqual(result.returncode, 0)
        fields = result.stdout.splitlines()[1].split(",")
        self.assertAlmostEqual(float(fields[1]), 1440/15.5, places=5)
        self.assertLess(float(fields[3]), float(fields[4]))

    def test_invalid_file(self):
        with tempfile.TemporaryDirectory() as directory:
            capture = pathlib.Path(directory)/"invalid.tle"
            for text in ("", "2 truncated\n"):
                capture.write_text(text)
                self.assertNotEqual(invoke(capture).returncode, 0)

    def test_missing_file(self):
        self.assertNotEqual(invoke(ROOT/"does-not-exist.tle").returncode, 0)

if __name__ == "__main__":
    unittest.main()
