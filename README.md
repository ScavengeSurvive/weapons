# weapons

An advanced, feature-rich, weapon framework for San Andreas Multiplayer.

Built around [ScavengeSurvive/item](https://github.com/ScavengeSurvive/item),
this package provides a way to declare ItemTypes as weapons using any GTA weapon
ID as a base. This allows multiple instances of the same base GTA weapon with
various individual characteristics such as effective stopping power and wound
infliction.

Damage is built on the advanced
[ScavengeSurvive/health](https://github.com/ScavengeSurvive/health) framework
which replaces "health" with: blood, bleeding, wounds and incapacitation.

**The package is currently a work in progress, the `todo` directory contains the
remaining modules from Scavenge and Survive which need to be re-structured for
this package.**

## Installation

Simply install to your project:

```bash
sampctl package install ScavengeSurvive/weapons
```

Include in your code and begin using the library:

```pawn
#include <weapons>
```

## Usage

Documentation is on the to-do list. Essentially, you bind an ItemType to a base
weapon with `DefineItemTypeWeapon` - you also need to declare ammunition types
and calibres.

## Testing

To test, simply run the package:

```bash
sampctl package run
```

And connect to `localhost:7777` with a friend/target to test.
