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
	Calibre:calibre_556,

	M9Pistol_1,
	PumpShotgun_1,
	M16Rifle_1,
	Ammo9mm_1,
	AmmoBuck_1,
	Ammo556_1,
	M9Pistol_2,
	PumpShotgun_2,
	M16Rifle_2,
	Ammo9mm_2,
	AmmoBuck_2,
	Ammo556_2,

	Text3D:debugM9Pistol_1,
	Text3D:debugPumpShotgun_1,
	Text3D:debugM16Rifle_1,
	Text3D:debugAmmo9mm_1,
	Text3D:debugAmmoBuck_1,
	Text3D:debugAmmo556_1,
	Text3D:debugM9Pistol_2,
	Text3D:debugPumpShotgun_2,
	Text3D:debugM16Rifle_2,
	Text3D:debugAmmo9mm_2,
	Text3D:debugAmmoBuck_2,
	Text3D:debugAmmo556_2;

main() {
	logger_debug("weapon", true);
	logger_debug("item", true);

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

	new AmmoType:Ammo9mm = DefineItemTypeAmmo(item_Ammo9mm, "Hollow Point", calibre_9mm, 1.0, 1.0, 0.2, 20);
	new AmmoType:AmmoBuck = DefineItemTypeAmmo(item_AmmoBuck, "No. 1", calibre_12g, 1.1, 1.8, 0.5, 24);
	new AmmoType:Ammo556 = DefineItemTypeAmmo(item_Ammo556, "FMJ", calibre_556, 1.1, 1.2, 0.8, 30);

	M9Pistol_1 = CreateItem(item_M9Pistol, 304.0, 1800.0, 16.8);
	PumpShotgun_1 = CreateItem(item_PumpShotgun, 304.0, 1803.0, 16.8);
	M16Rifle_1 = CreateItem(item_M16Rifle, 304.0, 1806.0, 16.8);
	Ammo9mm_1 = CreateItem(item_Ammo9mm, 306.0, 1800.0, 16.8);
	AmmoBuck_1 = CreateItem(item_AmmoBuck, 306.0, 1803.0, 16.8);
	Ammo556_1 = CreateItem(item_Ammo556, 306.0, 1806.0, 16.8);

	M9Pistol_2 = CreateItem(item_M9Pistol, 308.0, 1800.0, 16.8);
	PumpShotgun_2 = CreateItem(item_PumpShotgun, 308.0, 1803.0, 16.8);
	M16Rifle_2 = CreateItem(item_M16Rifle, 308.0, 1806.0, 16.8);
	Ammo9mm_2 = CreateItem(item_Ammo9mm, 310.0, 1800.0, 16.8);
	AmmoBuck_2 = CreateItem(item_AmmoBuck, 310.0, 1803.0, 16.8);
	Ammo556_2 = CreateItem(item_Ammo556, 310.0, 1806.0, 16.8);

	SetItemArrayDataAtCell(Ammo9mm_1, 100, 0, true);
	SetItemArrayDataAtCell(AmmoBuck_1, 100, 0, true);
	SetItemArrayDataAtCell(Ammo556_1, 100, 0, true);
	SetItemArrayDataAtCell(Ammo9mm_2, 1000, 0, true);
	SetItemArrayDataAtCell(AmmoBuck_2, 1000, 0, true);
	SetItemArrayDataAtCell(Ammo556_2, 1000, 0, true);

	SetWeaponAmmoType(M9Pistol_1, Ammo9mm);
	SetWeaponAmmoType(PumpShotgun_1, AmmoBuck);
	SetWeaponAmmoType(M16Rifle_1, Ammo556);
	SetWeaponAmmoType(M9Pistol_2, Ammo9mm);
	SetWeaponAmmoType(PumpShotgun_2, AmmoBuck);
	SetWeaponAmmoType(M16Rifle_2, Ammo556);

	SetWeaponReserve(M9Pistol_1, 20);
	SetWeaponReserve(PumpShotgun_1, 12);
	SetWeaponReserve(M16Rifle_1, 60);
	SetWeaponReserve(M9Pistol_2, 15);
	SetWeaponReserve(PumpShotgun_2, 7);
	SetWeaponReserve(M16Rifle_2, 36);

	debugM9Pistol_1 = Create3DTextLabel("text[]", 0xFFFF00FF, 304.0, 1800.0, 17.5, 100.0, 0);
	debugPumpShotgun_1 = Create3DTextLabel("text[]", 0xFFFF00FF, 304.0, 1803.0, 17.5, 100.0, 0);
	debugM16Rifle_1 = Create3DTextLabel("text[]", 0xFFFF00FF, 304.0, 1806.0, 17.5, 100.0, 0);
	debugAmmo9mm_1 = Create3DTextLabel("text[]", 0xFFFF00FF, 306.0, 1800.0, 17.5, 100.0, 0);
	debugAmmoBuck_1 = Create3DTextLabel("text[]", 0xFFFF00FF, 306.0, 1803.0, 17.5, 100.0, 0);
	debugAmmo556_1 = Create3DTextLabel("text[]", 0xFFFF00FF, 306.0, 1806.0, 17.5, 100.0, 0);

	debugM9Pistol_2 = Create3DTextLabel("text[]", 0xFFFF00FF, 308.0, 1800.0, 17.5, 100.0, 0);
	debugPumpShotgun_2 = Create3DTextLabel("text[]", 0xFFFF00FF, 308.0, 1803.0, 17.5, 100.0, 0);
	debugM16Rifle_2 = Create3DTextLabel("text[]", 0xFFFF00FF, 308.0, 1806.0, 17.5, 100.0, 0);
	debugAmmo9mm_2 = Create3DTextLabel("text[]", 0xFFFF00FF, 310.0, 1800.0, 17.5, 100.0, 0);
	debugAmmoBuck_2 = Create3DTextLabel("text[]", 0xFFFF00FF, 310.0, 1803.0, 17.5, 100.0, 0);
	debugAmmo556_2 = Create3DTextLabel("text[]", 0xFFFF00FF, 310.0, 1806.0, 17.5, 100.0, 0);

	// new itemName[MAX_ITEM_TEXT];
	// for(new ItemType:weaponItemType; IsValidItemType(weaponItemType); weaponItemType++) {
	// 	new WeaponType:weaponType;
	// 	if(GetItemTypeWeaponType(weaponItemType, weaponType)) {
	// 		continue;
	// 	}

	// 	new Calibre:weaponTypeCalibre;
	// 	GetWeaponTypeCalibre(weaponType, weaponTypeCalibre);
	// 	if(weaponTypeCalibre == NO_CALIBRE) {
	// 		continue;
	// 	}

	// 	new weaponItemName[MAX_ITEM_NAME];
	// 	GetItemTypeName(weaponItemType, weaponItemName);

	// 	new weaponItem = CreateItem(weaponItemType);

	// 	for(new ItemType:ammoItemType; IsValidItemType(ammoItemType); ammoItemType++) {
	// 		new AmmoType:ammoType;
	// 		if(GetItemTypeAmmoType(ammoItemType, ammoType)) {
	// 			continue;
	// 		}

	// 		new Calibre:ammoTypeCalibre;
	// 		if(GetAmmoTypeCalibre(ammoType, ammoTypeCalibre)) {
	// 			continue;
	// 		}

	// 		if(ammoTypeCalibre != weaponTypeCalibre) {
	// 			continue;
	// 		}

	// 		new mags;
	// 		new magSize;
	// 		GetWeaponTypeMaxReserveMags(weaponType, mags);
	// 		GetWeaponTypeMagSize(weaponType, magSize);

	// 		for(new round; round < magSize; round++) {
	// 			for(new mag; mag < mags * magSize; mag++) {
	// 				SetWeaponAmmoType(weaponItem, ammoType);
	// 				SetWeaponMagAmmo(weaponItem, round);
	// 				SetWeaponReserve(weaponItem, mag);
	// 				_weapons_generateItemName(weaponItem, itemName);
	// 				log("rendered",
	// 					_s("name", weaponItemName),
	// 					_s("text", itemName));
	// 			}
	// 		}
	// 	}

	// 	DestroyItem(weaponItem);
	// }
}

task update[1000]() {

	setWeaponDebugLabel(M9Pistol_1, debugM9Pistol_1);
	setWeaponDebugLabel(PumpShotgun_1, debugPumpShotgun_1);
	setWeaponDebugLabel(M16Rifle_1, debugM16Rifle_1);
	setWeaponDebugLabel(M9Pistol_2, debugM9Pistol_2);
	setWeaponDebugLabel(PumpShotgun_2, debugPumpShotgun_2);
	setWeaponDebugLabel(M16Rifle_2, debugM16Rifle_2);

	setAmmoDebugLabel(Ammo9mm_1, debugAmmo9mm_1);
	setAmmoDebugLabel(AmmoBuck_1, debugAmmoBuck_1);
	setAmmoDebugLabel(Ammo556_1, debugAmmo556_1);
	setAmmoDebugLabel(Ammo9mm_2, debugAmmo9mm_2);
	setAmmoDebugLabel(AmmoBuck_2, debugAmmoBuck_2);
	setAmmoDebugLabel(Ammo556_2, debugAmmo556_2);
}

setWeaponDebugLabel(itemid, Text3D:label) {
	new
		str[256],
		mag,
		reserve,
		total,
		AmmoType:ammoType;
	GetWeaponMagAmmo(itemid, mag);
	GetWeaponReserve(itemid, reserve);
	total = mag+reserve;
	GetWeaponAmmoType(itemid, ammoType);
	format(str, 256, "%d/%d (%d) - %d", mag, reserve, total, _:ammoType);
	Update3DTextLabelText(label, 0xFFFF00FF, str);
}

setAmmoDebugLabel(itemid, Text3D:label) {
	new
		str[256],
		amount;
	GetItemArrayDataAtCell(itemid, 0, amount);
	format(str, 256, "%d", amount);
	Update3DTextLabelText(label, 0xFFFF00FF, str);
}

hook OnItemArrayDataChanged(itemid) {
	// useful for debugging
	// log("item array data changed",
	// 	_i("ammoitem", _:a),
	// 	_i("itemid", itemid),
	// 	_i("length", GetItemArrayDataSize(itemid)));
}

public OnPlayerSpawn(playerid) {
	SetPlayerBlood(playerid, 100.0);
	return 1;
}

forward dummy();
public dummy() {
	new playerid;
	new amount;
	new remainder;
	GivePlayerAmmo(playerid, amount, remainder);
	new itemid;
	AddAmmoToWeapon(itemid, amount);

	new WeaponType:weapontype;
	new ItemType:itemtype;
	GetItemTypeWeaponType(ItemType:itemtype, weapontype);

	GetWeaponTypeItemType(weapontype, ItemType:itemtype);

	new base;
	GetWeaponTypeBaseWeapon(weapontype, base);

	new Calibre:calibre;
	GetWeaponTypeCalibre(weapontype, calibre);

	new Float:vel;
	GetWeaponTypeMuzzVelocity(weapontype, Float:vel);

	new magsize;
	GetWeaponTypeMagSize(weapontype, magsize);

	new reserve;
	GetWeaponTypeMaxReserveMags(weapontype, reserve);

	new animset;
	GetWeaponTypeAnimSet(weapontype, animset);

	new flags;
	GetWeaponTypeFlags(weapontype, flags);

	new ammo;
	GetWeaponMagAmmo(itemid, ammo);

	SetWeaponMagAmmo(itemid, amount);
	GetWeaponReserve(itemid, reserve);

	SetWeaponReserve(itemid, amount);
	new AmmoType:ammoType;
	GetWeaponAmmoType(itemid, ammoType);
	SetWeaponAmmoType(itemid, ammoType);
	new mag;
	GetPlayerMagAmmo(playerid, mag);

	GetPlayerReserveAmmo(playerid, reserve);

	GetPlayerTotalAmmo(playerid, ammo);
}