#include "weapons.inc"

#include <test-boilerplate-npcs>


new stock
	ItemType:item_M9Pistol,
	ItemType:item_PumpShotgun,
	ItemType:item_M16Rifle,
	ItemType:item_Ammo9mm,
	ItemType:item_AmmoBuck,
	ItemType:item_Ammo556,
	Calibre:calibre_9mm,
	Calibre:calibre_12g,
	Calibre:calibre_556;

main() {
	logger_debug("weapons", true);

	calibre_9mm = DefineAmmoCalibre("9mm", 0.25);
	calibre_12g = DefineAmmoCalibre("12 Gauge",	0.31);
	calibre_556 = DefineAmmoCalibre("5.56mm",	0.19);

	item_M9Pistol = DefineItemType("M9", "M9Pistol", 346, 1, 90.0, .maxhitpoints = 1);
	item_PumpShotgun = DefineItemType("Shotgun", "PumpShotgun", 349, 3, 90.0, .maxhitpoints = 3);
	item_M16Rifle = DefineItemType("M16", "M16Rifle", 356, 5, 90.0, .maxhitpoints = 5);
	item_Ammo9mm = DefineItemType("9mm Rounds", "Ammo9mm", 2037, 2, 0.0, 0.0, 0.0, 0.082, 0.221075, 0.067746, 0.037494, 87.375968, 305.182189, 5.691741, .maxhitpoints = 2);
	item_AmmoBuck = DefineItemType("Shotgun Shells", "AmmoBuck", 2038, 2, 0.0, 0.0, 0.0, 0.082,	0.221075, 0.067746, 0.037494, 87.375968, 305.182189, 5.691741, .maxhitpoints = 2);
	item_Ammo556 = DefineItemType("5.56 Rounds", "Ammo556", 2040, 3, 0.0, 0.0, 0.0, 0.082, 0.221075, 0.067746, 0.037494, 87.375968, 305.182189, 5.691741, .maxhitpoints = 3);

	DefineItemTypeWeapon(item_M9Pistol, WEAPON_COLT45, calibre_9mm, 330.0, 10, 1);
	DefineItemTypeWeapon(item_PumpShotgun, WEAPON_SHOTGUN, calibre_12g, 475.0, 6, 1);
	DefineItemTypeWeapon(item_M16Rifle, WEAPON_M4, calibre_556, 948.0, 30, 1);

	DefineItemTypeAmmo(item_Ammo9mm, "Hollow Point", calibre_9mm, 1.0, 1.0, 0.2, 20);
	DefineItemTypeAmmo(item_AmmoBuck, "No. 1", calibre_12g, 1.1, 1.8, 0.5, 24);
	DefineItemTypeAmmo(item_Ammo556, "FMJ", calibre_556, 1.1, 1.2, 0.8, 30);

	CreateItem(item_M9Pistol, 304.0, 1800.0, 16.8);
	CreateItem(item_PumpShotgun, 304.0, 1801.0, 16.8);
	CreateItem(item_M16Rifle, 304.0, 1802.0, 16.8);

	SetItemArrayDataAtCell(CreateItem(item_Ammo9mm, 305.0, 1800.0, 16.8), 100, 0, true);
	SetItemArrayDataAtCell(CreateItem(item_AmmoBuck, 305.0, 1801.0, 16.8), 100, 0, true);
	SetItemArrayDataAtCell(CreateItem(item_Ammo556, 305.0, 1802.0, 16.8), 100, 0, true);
}

public OnPlayerSpawn(playerid) {
	SetPlayerBlood(playerid, 100.0);
	return 1;
}

forward dummy();
public dummy() {
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