// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_ammunition_types
	#undef _inc_ammunition_types
#endif
// custom include-guard to ensure we don't duplicate
#if defined _ammunition_types_included
	#endinput
#endif
#define _ammunition_types_included


// MAX_ITEM_AMMO_TYPES controls the maximum amount of items that can be declared as ammunition types
#if !defined MAX_ITEM_AMMO_TYPES
	#define MAX_ITEM_AMMO_TYPES (AmmoType:20)
#endif

// MAX_AMMO_CALIBRE determines the maximum amount of ammunition calibres that can be created
#if !defined MAX_AMMO_CALIBRE
	#define MAX_AMMO_CALIBRE (Calibre:20)
#endif

// MAX_AMMO_CALIBRE_NAME controls the calibre name limit
#if !defined MAX_AMMO_CALIBRE_NAME
	#define MAX_AMMO_CALIBRE_NAME (32)
#endif


// NO_CALIBRE represents an invalid calibre type
#define NO_CALIBRE (Calibre:-1)

// INVALID_AMMUNITION_TYPE represents an invalid ammunition type
#define INVALID_AMMUNITION_TYPE (AmmoType:-1)


enum e_calibre_data {
	clbr_name[MAX_AMMO_CALIBRE_NAME],
	Float:clbr_bleedRate
}

enum E_ITEM_AMMO_DATA {
	ItemType:ammo_itemType,
	ammo_name[MAX_AMMO_CALIBRE_NAME],
	Calibre:ammo_calibre,
	Float:ammo_bleedrateMult,
	Float:ammo_knockoutMult,
	Float:ammo_penetration,
	ammo_size
}


static
	clbr_Data[MAX_AMMO_CALIBRE][e_calibre_data],
	clbr_Total,
	ammo_Data[MAX_ITEM_AMMO_TYPES][E_ITEM_AMMO_DATA],
	ammo_Total,
	AmmoType:ammo_ItemTypeAmmoType[MAX_ITEM_TYPE] = {INVALID_AMMUNITION_TYPE, ...},
	ItemType:ammo_ItemTypeLowerBound,
	ItemType:ammo_ItemTypeUpperBound;


// -
// API
// -


stock Calibre:DefineAmmoCalibre(name[], Float:bleedrate) {
	new Calibre:id = Calibre:clbr_Total++;
	strcat(clbr_Data[id][clbr_name], name, MAX_AMMO_CALIBRE_NAME);
	clbr_Data[id][clbr_bleedRate] = bleedrate;
	return id;
}

stock AmmoType:DefineItemTypeAmmo(ItemType:itemtype, name[], calibre, Float:bleedratemult, Float:knockoutmult, Float:penetration, size) {
	SetItemTypeMaxArrayData(itemtype, 1);

	ammo_Data[ammo_Total][ammo_itemType] = itemtype;
	strcat(ammo_Data[ammo_Total][ammo_name], name, MAX_AMMO_CALIBRE_NAME);
	ammo_Data[ammo_Total][ammo_calibre] = calibre;
	ammo_Data[ammo_Total][ammo_bleedrateMult] = bleedratemult;
	ammo_Data[ammo_Total][ammo_knockoutMult] = knockoutmult;
	ammo_Data[ammo_Total][ammo_penetration] = penetration;
	ammo_Data[ammo_Total][ammo_size] = size;

	ammo_ItemTypeAmmoType[itemtype] = ammo_Total;

	if(itemtype < ammo_ItemTypeLowerBound) {
		ammo_ItemTypeLowerBound = itemtype;
	} else if(itemtype > ammo_ItemTypeUpperBound) {
		ammo_ItemTypeUpperBound = itemtype;
	}

	return ammo_Total++;
}


// clbr_name
stock GetCalibreName(Calibre:calibre, name[MAX_AMMO_CALIBRE_NAME]) {
	if(!(Calibre:0 <= calibre < Calibre:clbr_Total)) {
		return 1;
	}

	name[0] = EOS;
	strcat(name, clbr_Data[calibre][clbr_name]);

	return 0;
}

// clbr_bleedRate
stock GetCalibreBleedRate(Calibre:calibre, &Float:bleedrate) {
	if(!(Calibre:0 <= calibre < Calibre:clbr_Total)) {
		return 1;
	}

	bleedrate = clbr_Data[calibre][clbr_bleedRate];
	return 0;
}

// ammo_itemType
stock GetAmmoTypeItemType(AmmoType:ammotype, ItemType:itemtype) {
	if(!(AmmoType:0 <= ammotype < AmmoType:ammo_Total)) {
		return 1;
	}

	itemtype = ammo_Data[ammotype][ammo_itemType];
	return 0;
}

// ammo_name
stock GetAmmoTypeName(AmmoType:ammotype, name[]) {
	if(!(AmmoType:0 <= ammotype < AmmoType:ammo_Total)) {
		return 1;
	}

	name[0] = EOS;
	strcat(name, ammo_Data[ammotype][ammo_name], MAX_AMMO_CALIBRE_NAME);

	return 0;
}

// ammo_calibre
stock GetAmmoTypeCalibre(AmmoType:ammotype, &Calibre:calibre) {
	if(!(AmmoType:0 <= ammotype < AmmoType:ammo_Total)) {
		return 1;
	}

	calibre = ammo_Data[ammotype][ammo_calibre];
	return 0;
}

// ammo_bleedrateMult
stock GetAmmoTypeBleedrateMultiplier(AmmoType:ammotype, &Float:bleedrateMult) {
	if(!(AmmoType:0 <= ammotype < AmmoType:ammo_Total)) {
		return 1;
	}

	bleedrateMult = ammo_Data[ammotype][ammo_bleedrateMult];
	return 0;
}

// ammo_knockoutMult
stock GetAmmoTypeKnockoutMultiplier(AmmoType:ammotype, &Float:knockoutMult) {
	if(!(AmmoType:0 <= ammotype < AmmoType:ammo_Total)) {
		return 1;
	}

	knockoutMult = ammo_Data[ammotype][ammo_knockoutMult];
	return 0;
}

// ammo_penetration
stock GetAmmoTypePenetration(AmmoType:ammotype, &Float:penetration) {
	if(!(AmmoType:0 <= ammotype < AmmoType:ammo_Total)) {
		return 1;
	}

	penetration = ammo_Data[ammotype][ammo_penetration];
	return 0;
}

// ammo_size
stock GetAmmoTypeSize(AmmoType:ammotype, &size) {
	if(!(AmmoType:0 <= ammotype < AmmoType:ammo_Total)) {
		return 1;
	}

	size = ammo_Data[ammotype][ammo_size];
	return 0;
}


stock GetItemTypeAmmoType(ItemType:itemtype, &AmmoType:ammotype) {
	if(!IsValidItemType(itemtype)) {
		return 1;
	}

	ammotype = ammo_ItemTypeAmmoType[itemtype];
	return 0;
}

stock GetAmmoItemTypesOfCalibre(calibre, ItemType:output[], max = sizeof(output)) {
	new idx;

	for(new i; i < ammo_Total && idx < max; i++) {
		if(ammo_Data[i][ammo_calibre] == calibre) {
			output[idx++] = ammo_Data[i][ammo_itemType];
		}
	}

	return idx;
}


// -
// Internal
//


hook OnItemNameRender(itemid, ItemType:itemtype) {
	new AmmoType:ammotype = ammo_ItemTypeAmmoType[itemtype];

	if(ammotype == INVALID_AMMUNITION_TYPE) {
		return Y_HOOKS_CONTINUE_RETURN_0;
	}

	new
		amount,
		str[MAX_ITEM_TEXT];
	
	GetItemExtraData(itemid, amount);

	format(str, sizeof(str), "%d, %s, %s",
		amount,
		clbr_Data[ammo_Data[ammotype][ammo_calibre]][clbr_name],
		ammo_Data[ammotype][ammo_name]);

	SetItemNameExtra(itemid, str);

	return Y_HOOKS_CONTINUE_RETURN_0;
}

// todo: move to SS core codebase
// hook OnItemCreate(itemid) {
// 	if(GetItemLootIndex(itemid) != -1) {
// 		new ammotype = GetItemTypeAmmoType(GetItemType(itemid));
// 		if(ammotype != -1) {
// 			SetItemExtraData(itemid, ammo_Data[ammotype][ammo_size] == 1 ? random(1) : random(ammo_Data[ammotype][ammo_size] - 1) + 1);
// 		}
// 	}
// }
