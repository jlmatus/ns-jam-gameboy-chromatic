"""Render the deliberately authored 32x32 MATUS cells; never resample a photo."""
import json
import pathlib
import struct
import zlib

ROOT = pathlib.Path(__file__).resolve().parent.parent
SOURCE = ROOT / "art/portraits/matus-v2"


def png(path, cells, palette, scale=1):
    def chunk(kind, data):
        return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data))

    pixels = b"".join(
        (b"\0" + b"".join(bytes.fromhex(palette[c].lstrip("#")) * scale for c in row)) * scale
        for row in cells
    )
    path.write_bytes(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 32 * scale, 32 * scale, 8, 6, 0, 0, 0))
                     + chunk(b"IDAT", zlib.compress(pixels)) + chunk(b"IEND", b""))


if __name__ == "__main__":
    spec = json.loads((SOURCE / "pixels.json").read_text())
    cells = [["."] * 32 for _ in range(32)]
    assert len(spec["rows"]) == 32
    for y, (offset, row) in enumerate(spec["rows"]):
        assert 0 <= offset <= 32 and offset + len(row) <= 32
        for x, c in enumerate(row, offset):
            assert c in spec["palette"]
            cells[y][x] = c
    png(SOURCE / "matus-32.png", cells, spec["palette"])
    png(SOURCE / "matus-preview.png", cells, spec["palette"], 12)
    native = {k: v + "FF" for k, v in spec["nativeSourceMapping"].items()}
    png(SOURCE / "matus-native-source.png", cells, native)
