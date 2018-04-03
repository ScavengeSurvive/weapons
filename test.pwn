#include "weapons.inc"


new stock
	ItemType:item_GolfClub,
	ItemType:item_M9Pistol,
	ItemType:item_RocketLauncher,
	ItemType:item_Flamer;

main() {
	new Calibre:calibre_9mm = DefineAmmoCalibre("9mm", 0.25);
	new Calibre:calibre_rpg = DefineAmmoCalibre("RPG", 0.0);

	item_GolfClub = DefineItemType("Golf Club", "GolfClub", 333, 3, 90.0, .maxhitpoints = 3);
	DefineItemTypeWeapon(item_GolfClub, WEAPON_GOLFCLUB, NO_CALIBRE, 0.07, 35, 0);

	item_M9Pistol = DefineItemType("M9", "M9Pistol", 346, 1, 90.0, .maxhitpoints = 1);
	DefineItemTypeWeapon(item_M9Pistol, WEAPON_COLT45, calibre_9mm, 330.0, 10, 1);

	item_RocketLauncher = DefineItemType("RPG", "RocketLauncher", 359, 6, 90.0, .maxhitpoints = 6);
	DefineItemTypeWeapon(item_RocketLauncher, WEAPON_ROCKETLAUNCHER, calibre_rpg, 0.0, 1, 0, -1, WEAPON_FLAG_ASSISTED_FIRE_ONCE | WEAPON_FLAG_ONLY_FIRE_AIMED);

	// item_Flamer = DefineItemType("Flamer", "Flamer", 361, 7, 90.0, .maxhitpoints = 7);
	// DefineItemTypeWeapon(item_Flamer, WEAPON_FLAMETHROWER, liquid_Petrol, 0.0, 100, 10, -1, WEAPON_FLAG_ASSISTED_FIRE | WEAPON_FLAG_LIQUID_AMMO);

	new playerid;
	new amount;
	new itemid;
	GivePlayerAmmo(playerid, amount);
	AddAmmoToWeapon(itemid, amount);

	new WeaponType:weapontype;
	new ItemType:itemtype;
	GetItemTypeWeapon(ItemType:itemtype, weapontype);

	GetItemWeaponItemType(weapontype, ItemType:itemtype);

	new base;
	GetItemWeaponBaseWeapon(weapontype, base);

	new Calibre:calibre;
	GetItemWeaponCalibre(weapontype, calibre);

	new Float:vel;
	GetItemWeaponMuzzVelocity(weapontype, Float:vel);

	new magsize;
	GetItemWeaponMagSize(weapontype, magsize);

	new reserve;
	GetItemWeaponMaxReserveMags(weapontype, reserve);

	new animset;
	GetItemWeaponAnimSet(weapontype, animset);

	new flags;
	GetItemWeaponFlags(weapontype, flags);

	GetItemTypeWeaponBaseWeapon(itemtype, base);

	GetItemTypeWeaponCalibre(itemtype, calibre);

	GetItemTypeWeaponMuzzVelocity(ItemType:itemtype, Float:vel);

	GetItemTypeWeaponMagSize(ItemType:itemtype, magsize);

	GetItemTypeWeaponMaxReserveMags(ItemType:itemtype, reserve);

	GetItemTypeWeaponAnimSet(ItemType:itemtype, animset);

	GetItemTypeWeaponFlags(ItemType:itemtype, flags);

	new ammo;
	GetItemWeaponItemMagAmmo(itemid, ammo);

	SetItemWeaponItemMagAmmo(itemid, amount);
	GetItemWeaponItemReserve(itemid, reserve);

	SetItemWeaponItemReserve(itemid, amount);
	GetItemWeaponItemAmmoItem(itemid, ItemType:itemtype);
	SetItemWeaponItemAmmoItem(itemid, ItemType:itemtype);
	new mag;
	GetPlayerMagAmmo(playerid, mag);

	GetPlayerReserveAmmo(playerid, reserve);

	GetPlayerTotalAmmo(playerid, ammo);
}
