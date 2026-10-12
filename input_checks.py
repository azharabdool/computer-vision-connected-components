"""New portfolio verification; only authored synthetic rasters are used."""
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent
EXECUTABLE = Path(sys.argv[1]).resolve()


def run(*arguments, expected=0):
    result = subprocess.run(
        [str(EXECUTABLE), *map(str, arguments)], cwd=ROOT,
        capture_output=True, text=True, timeout=10,
    )
    assert result.returncode == expected, (arguments, result.returncode, result.stderr)
    return result


with tempfile.TemporaryDirectory(prefix='component-checks-') as folder:
    folder = Path(folder)
    bad = folder / 'bad.pgm'
    for data in [b'P2\n1 1\n255\n0', b'P5\n-1 1\n255\n',
                 b'P5\n2 2\n255\n\x00', b'P5\n1 1\n65535\n\x00']:
        bad.write_bytes(data)
        run(bad, expected=2)
    for arguments in [[], ['-t', '999', 'absent.pgm'],
                      ['-s', '9', '2', 'absent.pgm'], ['-t', 'abc', 'absent.pgm'],
                      ['-w', 'absent.pgm'], ['absent.pgm']]:
        run(*arguments, expected=2)
    print('10 malformed-input checks passed')

    source = ROOT / 'examples/synthetic-input.pgm'
    run('-t', '0', source, expected=2)
    one = run('-s', '610', '1000', '-p', source)
    assert 'Total number of components = 1' in one.stdout
    assert 'Size of largest component = 625' in one.stdout
    empty = run('-s', '1000', '2000', '-p', source)
    assert 'Total number of components = 0' in empty.stdout
    output = folder / 'mask.pgm'
    run('-t', '128', '-s', '20', '1000', '-w', output, source)
    assert output.read_bytes() == (ROOT / 'examples/synthetic-mask.pgm').read_bytes()
    print('4 additional safety/output checks passed')
