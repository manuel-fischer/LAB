# JSON API
The LAB JSON API allows to create custom blocks, worlds and more.

## World Generation
### Flat

```json
{
    "@type": "Flat",
    "block": "STONE"
}
```

### Overworld

```json
{
    "@type": "Overworld",
    "layers": [
        {
            "@type": "Terrain",
            "top": "grass",
            "fade": "dirt"
        },
        {
            "@type": "Cave",
            "factor": 0.5,
            "block": "air"
        },
        {
            "@type": "Hanging",
            "max_count": 7,
            "max_height": 15,
            "block": "blue_light"
        },
        {
            "@type": "Veins",
            "prob": 0.125,
            "orig": 0.5,
            "block": "marble"
        }
    ]
}
```


## Blocks

