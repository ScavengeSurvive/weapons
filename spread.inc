#include <linegen>
#include <YSI\y_hooks>


hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:shotX, Float:shotY, Float:shotZ) {
    new
        Float:camX, Float:camY, Float:camZ,
        Float:cx,
        Float:cy,
        Float:cz,
        Float:bx,
        Float:by,
        Float:bz,
        Float:ox,
        Float:oy,
        Float:oz,
        Float:distance;

    GetPlayerCameraPos(playerid, camX, camY, camZ);
    GetPlayerLastShotVectors(playerid, ox, oy, oz, bx, by, bz);
    distance = GetDistance3D(ox, oy, oz, bx, by, bz);
    GetPointInFrontOfCamera3D(playerid, cx, cy, cz, distance);

    CreateLineSegment(
        19087, 2.46,
        camX, camY, camZ,
        shotX, shotY, shotZ,
        90.0, 0.0, 0.0,
        -(2.46 / 2.0),
        .maxlength = 300.0
    );

    Streamer_Update(playerid);

    return true;
}

//functions

static GetPointInFrontOfCamera3D(playerid, &Float:tx, &Float:ty, &Float:tz, Float:radius, &Float:rx=0.0, &Float:rz=0.0) {
    new Float:x, Float:y, Float:z;
    GetPlayerCameraPos(playerid, x, y, z);
    GetPlayerCameraRotation(playerid, rx, rz);
    GetPointInFront3D(x, y, z, rx, rz, radius, tx, ty, tz);
}

static GetPlayerCameraRotation(playerid, &Float:rx, &Float:rz) {
    new Float:mx, Float:my, Float:mz;
    GetPlayerCameraFrontVector(playerid, mx, my, mz);
    rx = GetAbsoluteAngle(-(acos(mz)-90.0));
    rz = GetAbsoluteAngle((atan2(my, mx)-90.0));
}

static GetPointInFront3D(Float:x, Float:y, Float:z, Float:rx, Float:rz, Float:radius, &Float:tx, &Float:ty, &Float:tz) {
    tx = x - (radius * floatcos(rx, degrees) * floatsin(rz, degrees));
    ty = y + (radius * floatcos(rx, degrees) * floatcos(rz, degrees));
    tz = z + (radius * floatsin(rx, degrees));
}