from PIL import Image

IMAGES = """
    bricks
    cobble
    layered_stone
    layered_stone_top
    polished_stone
    smooth_stone
    stone
    stone_cracked
""".split()

for name in IMAGES:
    src = f"src_{name}.png"
    dst = f"{name}.png"

    img = Image.open(src).convert("L")
    dat = img.load()

    if 0:
        min_col = 255
        max_col = 0
        for y in range(img.height):
            for x in range(img.width):
                c = dat[x, y]
                min_col = min(min_col, c)
                max_col = max(max_col, c)

        print(name, min_col, max_col)
    else:
        min_col = 128
        max_col = 255

    # remap
    for y in range(img.height):
        for x in range(img.width):
            c0 = dat[x, y]
            c = (c0-min_col)*256//(max_col-min_col)
            assert 0 <= c < 256
            #assert abs(c0 - (c+255+1)//2) < 1
            dat[x, y] = c

    img.save(dst)
