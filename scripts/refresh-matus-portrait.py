"""Render explicitly authored MATUS cells; never resample a photo or study."""
import argparse
import json
import pathlib
import struct
import zlib

ROOT = pathlib.Path(__file__).resolve().parent.parent


def png(path, cells, palette, scale=1):
    def chunk(kind, data):
        return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data))

    pixels = b"".join(
        (b"\0" + b"".join(bytes.fromhex(palette[c].lstrip("#")) * scale for c in row)) * scale
        for row in cells
    )
    path.write_bytes(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", len(cells[0]) * scale, len(cells) * scale, 8, 6, 0, 0, 0))
                     + chunk(b"IDAT", zlib.compress(pixels)) + chunk(b"IEND", b""))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", default="matus-v2")
    source = ROOT / "art/portraits" / parser.parse_args().source
    for filename, prefix in [("pixels.json", "matus"), ("card-pixels.json", "matus-card")]:
        if not (source / filename).exists():
            continue
        spec = json.loads((source / filename).read_text())
        width, height = spec["width"], spec["height"]
        cells = [["."] * width for _ in range(height)]
        assert len(spec["rows"]) == height
        for y, (offset, row) in enumerate(spec["rows"]):
            assert 0 <= offset <= width and offset + len(row) <= width
            for x, c in enumerate(row, offset):
                assert c in spec["palette"]
                cells[y][x] = c
        png(source / f"{prefix}-{width}.png", cells, spec["palette"])
        png(source / f"{prefix}-preview.png", cells, spec["palette"], 12 if width==32 else 8)
        if len(spec["palette"])-1 <= 3:
            native = {k: v + "FF" for k, v in spec["nativeSourceMapping"].items()}
            png(source / f"{prefix}-native-source.png", cells, native)
