
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

enum CameraMode
{
	MODE_BEHINDCAR = 3,
	MODE_CAM_ON_A_STRING = 18,
};

int16 &DebugCamMode = *(int16*)0x95CCF2;
WRAPPER void CCam::DoAverageOnVector(CVector *out, CVector *in) { EAXJMP(0x457710); }
WRAPPER float CCam::GetPedBetaAngleForClearView(CVector &, float, float, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies) { EAXJMP(0x458060); }
//WRAPPER void CCam::WorkOutCamHeightWeeCar(CVector &targetCoors, float targetOrientation) { EAXJMP(0x4662D0); }

WRAPPER void *CPad::GetWeapon(void) { EAXJMP(0x4936C0); }
WRAPPER bool CPad::ForceCameraBehindPlayer(void) { EAXJMP(0x493D80); }

CEntity *&CWorld__pIgnoreEntity = *(CEntity**)0x8F6494;

namespace CMaths
{
float ATan2(float x, float y) { return atan2f(x, y); }
float Cos(float a) { return cosf(a); }
float Sin(float a) { return sinf(a); }
float Sqrt(float x) { return sqrtf(x); }
};

float Magnitude2D(CVector &v) { return sqrt(v.x * v.x + v.y * v.y); }
float Magnitude(CVector &v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }


float FOV = 80.0f;	// default 70

WRAPPER void WellBufferMe(float TheTarget, float* TheValueToChange, float* ValueSpeedSoFar, float TopSpeed, float SpeedStep, bool IsAnAngle) { EAXJMP(0x456F40); }

WRAPPER bool CWorld__ProcessLineOfSight(CVector const &,CVector const &,CColPoint &,CEntity *&, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies,bool,bool) { EAXJMP(0x4AF970); }
WRAPPER bool CWorld__ProcessVerticalLine(CVector const &, float,CColPoint &,CEntity *&, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies,bool,bool) { EAXJMP(0x4B0DE0); }
WRAPPER float CWorld__FindRoofZFor3DCoord(float x, float y, float z, bool *foundRoof) { EAXJMP(0x4B3B50); }
WRAPPER float CWorld__FindGroundZFor3DCoord(float x, float y, float z, bool *foundGround) { EAXJMP(0x4B3AE0); }

void
CCam::MyProcess_FollowPed(const CVector &ThisCamsTarget, float TargetOrientation,
							   float SpeedVar, float SpeedVarDesired)
{
	float fUpLimit = DEGTORAD(85.0f);
	float fDownLimit = DEGTORAD(45.0f);

	FOV = ::FOV;

	if(ResetStatics){
		m_bCollisionChecksOn = true;
	}

	CVector TargetCoors = ThisCamsTarget;

	TargetCoors.z += m_fSyphonModeTargetZOffSet;
	CVector TempTarget;
	DoAverageOnVector(&TempTarget, &TargetCoors);
	TargetCoors = TempTarget;
//	TargetCoors.z += m_unknownZOffset;	// this causes twitches, hm...

	CVector Dist = Source - TargetCoors;
	float Length = Magnitude(Dist);


	float IdealLength;
	if(TheCamera.PedZoomIndicator == 1.0) IdealLength = 2.090556f;
	if(TheCamera.PedZoomIndicator == 2.0) IdealLength = 3.34973f;
	if(TheCamera.PedZoomIndicator == 3.0) IdealLength = 4.704914f;
	if(TheCamera.PedZoomIndicator == 4.0) IdealLength = 2.090556f;

//	m_fRealGroundDist = 0.0f;
//	m_fMinDistAwayFromCamWhenInterPolating = m_fRealGroundDist;

	if(Length != 0.0f){
		Dist.x *= IdealLength/Length;
		Dist.y *= IdealLength/Length;
		Dist.z *= IdealLength/Length;
	}else{
		Dist.x = 1.0f;
		Dist.y = 1.0f;
		Dist.z = 0.0f;
	}

	Length = Magnitude(Dist);

	float groundDist = Magnitude2D(Dist);
	Beta = CGeneral::GetATanOfXY(Dist.x, Dist.y);
	Alpha = CGeneral::GetATanOfXY(groundDist, Dist.z);

	while(Beta >= PI) Beta -= 2.0f*PI;
	while(Beta < -PI) Beta += 2.0f*PI;
	while(Alpha >= PI) Alpha -= 2.0f*PI;
	while(Alpha < -PI) Alpha += 2.0f*PI;

	float StickBeta = -CPad::GetPad(0)->GetRightStickX()/1280.0f;
	float StickAlpha = CPad::GetPad(0)->GetRightStickY()/1280.0f;

//	Beta = TargetOrientation + PI;

	Beta += StickBeta;
	Alpha += StickAlpha;

	if(Alpha > fUpLimit) Alpha = fUpLimit;
	if(Alpha < -fDownLimit) Alpha = -fDownLimit;

//	Beta = GetPedBetaAngleForClearView(TargetCoors, something, 0.0, 1, 0, 0, 1, 0);

	// clip before applying rotation from pad
//	float beta1 = GetPedBetaAngleForClearView(TargetCoors, groundDist, 0.0f, true, false, false, true, false);
//	Beta += beta1;

	// now rotate as far as we want and can
//	float a = DEGTORAD(5.0f);
//	float savedBeta = Beta;
//	float b1 = GetPedBetaAngleForClearView(TargetCoors, groundDist, a, true, false, false, true, false);
//	if(b1 != 0.0f){
//	}else{
//		float b2 = GetPedBetaAngleForClearView(TargetCoors, groundDist, -a, true, false, false, true, false);
//		if(b2 != 0.0f){
//		}
//	}



	Dist = CVector(CMaths::Cos(Alpha)*CMaths::Cos(Beta), CMaths::Cos(Alpha)*CMaths::Sin(Beta), CMaths::Sin(Alpha));
	Dist = Dist * Length;

	// Cull zone effects:
	//	m_fCloseInPedHeightOffset
	//	m_fCloseInCarHeightOffset
// to avoid peds and cars:
//	Dist.z = m_fPedBetweenCameraHeightOffset;
//	Dist.z = m_fDimensionOfHighestNearCar;


//	float zoffset = MAX(m_fPedBetweenCameraHeightOffset, m_fDimensionOfHighestNearCar);
//	Dist.z = MAX(Dist.z, zoffset);


	Source = TargetCoors + Dist;

	CColPoint colpoint;
	CEntity *e = nil;

	if(CWorld__ProcessLineOfSight(TargetCoors, Source, colpoint, e, true, true, false, true, false, true, true)){
		Source = colpoint.point;

		CVector d = Source - TargetCoors;
		float l = d.Magnitude();
		if(l > 0.4f)
			d = d/l*(l-0.3f);
		Source = TargetCoors + d;

		if(Magnitude2D(TargetCoors - Source) < 2.0f)
			RwCameraSetNearClipPlane(Scene.camera, 0.05f);
	}

	CWorld__pIgnoreEntity = CamTargetEntity;
	if(CWorld__ProcessLineOfSight(TargetCoors, Source, colpoint, e, true, true, true, true, false, true, true)){
		Source = colpoint.point;

		CVector d = Source - TargetCoors;
		float l = d.Magnitude();
		if(l > 0.4f)
			d = d/l*(l-0.3f);
		Source = TargetCoors + d;

		if(Magnitude2D(TargetCoors - Source) < 2.0f)
			RwCameraSetNearClipPlane(Scene.camera, 0.05f);
	}
	CWorld__pIgnoreEntity = nil;

	Front = TargetCoors - Source;
	Front.Normalise();
	GetVectorsReadyForRW();
	
	ResetStatics=FALSE;
}

//
//
//
// Vehicle Cam
//
//
//

void
CCam::WorkOutCamHeightWeeCar(CVector &targetCoors, float targetOrientation)
{
	float targetZOffset = 0.0f;
	static bool unk = false;
	static float zOffset = 0.0f;
	static float zOffsetSpeed = 0.0f;

	if(ResetStatics){
		zOffset = 0.0f;
		zOffsetSpeed = 0.0f;
		Alpha = DEGTORAD(25.0f);
		AlphaSpeed = 0.0f;
	}
	// TODO: if no rain zone: 14
	float alphaTarget = DEGTORAD(25.0f);
	WellBufferMe(alphaTarget, &Alpha, &AlphaSpeed, 0.1f, 0.05f, true);
	Source.z = targetCoors.z + CA_MAX_DISTANCE*sin(Alpha);

	if(FindPlayerVehicle()){
		CColPoint colpoint;
		CEntity *ent;

		m_unknownZOffset = 0.0f;
		bool collisionBelow = false;
		bool collisionAbove = false;
		float collisionBelowZ = 0.0f;
		float collisionAboveZ = 0.0f;

		float collisionZ = 0.0f;
		if(CWorld__ProcessVerticalLine(Source, -1000.0f, colpoint, ent, true, false, false, false, false, false, false) &&
		   (ent->bfTypeStatus & 7) == 1){
			collisionBelow = true;
			collisionBelowZ = colpoint.point.z;
		}

		if(collisionBelow){
			if(Source.z - collisionBelowZ < 0.9f){
				unk = true;
				targetZOffset = collisionBelowZ + 0.9f - Source.z;
			}else{
				if(m_bCollisionChecksOn)
					unk = false;
				else
					targetZOffset = 0.0f;
			}
		}else{
			if(CWorld__ProcessVerticalLine(Source, 1000.0f, colpoint, ent, true, false, false, false, false, false, false) &&
			   (ent->bfTypeStatus & 7) == 1){
				collisionAbove = true;
				collisionAboveZ = colpoint.point.z;
			}
			if(collisionAbove){
				if(Source.z - collisionAboveZ < 0.9f){
					unk = true;
					targetZOffset = collisionAboveZ + 0.9f - Source.z;
				}else{
					if(m_bCollisionChecksOn)
						unk = false;
					else
						targetZOffset = 0.0f;
				}
			}
		}
	}

	if(targetZOffset > zOffset)
		zOffset = targetZOffset;
	else
		WellBufferMe(targetZOffset, &zOffset, &zOffsetSpeed, 0.27f, 0.1f, false);
	// TODO: do stuff according to piece type

	Source.z += zOffset;
}

// Avoid clipping through buildings
#if 0
WRAPPER bool CCam::FixCamIfObscured(CVector &targetCoors, float xx, float targetOrientation) { EAXJMP(0x457B90); }
#else
bool
CCam::FixCamIfObscured(CVector &targetCoors, float zz, float targetOrientation)
{
	CVector target = targetCoors;
	bool useEntityPos = false;
	CVector entityPos;
	static CColPoint colPoint;
	static bool LastObscured = false;

	if(Mode == MODE_BEHINDCAR)
		target.z += zz/2.0f;
	if(Mode == MODE_CAM_ON_A_STRING){
		useEntityPos = true;
		target.z += zz/2.0f;
		entityPos = *(CVector*)&CamTargetEntity->matrix.matrix.pos;
	}

	CVector source = Source;

	bool obscured1 = false;
	bool obscured2 = false;
	bool fix1 = false;
	float dist1 = 0.0f;
	float dist2 = 0.0f;
	CEntity *ent;
	if(m_bCollisionChecksOn || LastObscured){
		obscured1 = CWorld__ProcessLineOfSight(target, source, colPoint, ent, true, false, false, true, false, true, true);
		if(obscured1){
			dist1 = Magnitude2D(target - colPoint.point);
			fix1 = true;
			if(useEntityPos)
				obscured1 = CWorld__ProcessLineOfSight(entityPos, source, colPoint, ent, true, false, false, true, false, true, true);
		}else if(m_bFixingBeta){
			float d = Magnitude(source - target);
			source.x = target.x - d*cos(targetOrientation);
			source.y = target.y - d*sin(targetOrientation);

			// same check again
			obscured2 = CWorld__ProcessLineOfSight(target, source, colPoint, ent, true, false, false, true, false, true, true);
			if(obscured2){
				dist2 = Magnitude2D(target - colPoint.point);
				if(useEntityPos)
					obscured2 = CWorld__ProcessLineOfSight(entityPos, source, colPoint, ent, true, false, false, true, false, true, true);
			}
		}
		LastObscured = obscured1 || obscured2;
	}

	// nothing to do
	if(!LastObscured)
		return false;

	if(fix1){
		Source.x = target.x - cos(Beta)*dist1;
		Source.y = target.y - sin(Beta)*dist1;
		if(Mode == MODE_BEHINDCAR)
			Source = colPoint.point;
	}else{
		WellBufferMe(dist2, &m_fDistanceBeforeChanges, &DistanceSpeed, 0.2f, 0.025f, false);
		Source.x = target.x - cos(Beta)*m_fDistanceBeforeChanges;
		Source.y = target.y - sin(Beta)*m_fDistanceBeforeChanges;
	}

	if(ResetStatics){
		m_fDistanceBeforeChanges = Magnitude2D(Source - target);
		DistanceSpeed = 0.0f;
		Source.x = colPoint.point.x;
		Source.y = colPoint.point.y;
	}
	return true;
}
#endif

// Make sure the camera points forward when we're driving forward
#if 0
WRAPPER bool CCam::RotCamIfInFrontCar(CVector &targetCoors, float targetOrientation) { EAXJMP(0x465DA0); }
#else
bool
CCam::RotCamIfInFrontCar(CVector &targetCoors, float targetOrientation)
{
	bool movingForward = false;
	CPhysical *phys = (CPhysical*)CamTargetEntity;

	float forwardSpeed = DotProduct(phys->matrix.GetForward(), phys->GetSpeed(CVector(0.0f, 0.0f, 0.0f)));
	if(forwardSpeed > 0.02f)
		movingForward = true;

	float dist = Magnitude2D(Source - targetCoors);

	float dbeta = targetOrientation - Beta;
	while(dbeta > PI) dbeta -= 2*PI;
	while(dbeta < -PI) dbeta += 2*PI;

	if(abs(dbeta) > DEGTORAD(20.0f) && movingForward && TheCamera.m_uiTransitionState == 0)
		m_bFixingBeta = true;

	CPad *pad = CPad::GetPad(0);
	if(!(pad->GetLookBehindForCar() || pad->GetLookBehindForPed() || pad->GetLookLeft() || pad->GetLookRight()))
		if(DirectionWasLooking != 3)
			TheCamera.m_bCamDirectlyBehind = true;

	if(!m_bFixingBeta && !TheCamera.m_bUseTransitionBeta && !TheCamera.m_bCamDirectlyBehind && !TheCamera.m_bCamDirectlyInFront)
		return false;

	bool doThing = false;
	if(TheCamera.m_bCamDirectlyBehind || TheCamera.m_bCamDirectlyInFront || TheCamera.m_bUseTransitionBeta)
		if(&TheCamera.Cams[TheCamera.ActiveCam] == (CCam_public*)this)
			doThing = true;

	if(m_bFixingBeta || doThing){
		WellBufferMe(targetOrientation, &Beta, &BetaSpeed, 0.15f, 0.007f, true);

		if(TheCamera.m_bCamDirectlyBehind && &TheCamera.Cams[TheCamera.ActiveCam] == (CCam_public*)this)
			Beta = targetOrientation;
		if(TheCamera.m_bCamDirectlyInFront && &TheCamera.Cams[TheCamera.ActiveCam] == (CCam_public*)this)
			Beta = targetOrientation + PI;
		if(TheCamera.m_bUseTransitionBeta && &TheCamera.Cams[TheCamera.ActiveCam] == (CCam_public*)this)
			Beta = m_fTransitionBeta;

		Source.x = targetCoors.x - cos(Beta)*dist;
		Source.y = targetCoors.y - sin(Beta)*dist;

		// Check if we're done
		dbeta = targetOrientation - Beta;
		while(dbeta > PI) dbeta -= 2*PI;
		while(dbeta < -PI) dbeta += 2*PI;
		if(abs(dbeta) < DEGTORAD(2.0f))
			m_bFixingBeta = false;
	}
	TheCamera.m_bCamDirectlyBehind = false;
	TheCamera.m_bCamDirectlyInFront = false;
	return true;
}
#endif

// Is this ever used?
void
CCam::Process_BehindCar(const CVector &ThisCamsTarget, float TargetOrientation, float SpeedVar, float SpeedVarDesired)
{
	FOV = ::FOV;

	if((CamTargetEntity->bfTypeStatus & 7) != 2)
		return;

	CVector targetcoors = ThisCamsTarget;
	targetcoors.z -= 0.2f;
	CA_MAX_DISTANCE = 9.95f;
	CA_MIN_DISTANCE = 8.5f;

	CVector dist = Source - targetcoors;
	float distMag = Magnitude2D(dist);
	m_fDistanceBeforeChanges = distMag;
	if(distMag < 0.002f)
		distMag = 0.002f;
	Beta = CGeneral::GetATanOfXY(targetcoors.x - Source.x, targetcoors.y - Source.y);
	if(distMag > CA_MAX_DISTANCE){
		Source.x = targetcoors.x + dist.x/distMag * CA_MAX_DISTANCE;
		Source.y = targetcoors.y + dist.y/distMag * CA_MAX_DISTANCE;
	}else if(distMag < CA_MIN_DISTANCE){
		Source.x = targetcoors.x + dist.x/distMag * CA_MIN_DISTANCE;
		Source.y = targetcoors.y + dist.y/distMag * CA_MIN_DISTANCE;
	}
	targetcoors.z += 0.8f;

	WorkOutCamHeightWeeCar(targetcoors, TargetOrientation);
	RotCamIfInFrontCar(targetcoors, TargetOrientation);
	FixCamIfObscured(targetcoors, 1.2f, TargetOrientation);

	Front = targetcoors - Source;
	m_cvecTargetCoorsForFudgeInter = targetcoors;
	ResetStatics = false;
	GetVectorsReadyForRW();
}

// Standard cam on a string algorithm
#if 0
WRAPPER void CCam::Cam_On_A_String_Unobscured(const CVector &target, float distval) { EAXJMP(0x457210); }
#else
void
CCam::Cam_On_A_String_Unobscured(const CVector &target, float distval)
{
	CA_MAX_DISTANCE = distval + 0.1f + TheCamera.CarZoomValueSmooth;
	CA_MIN_DISTANCE = MIN(distval*0.6f, 3.5f);

	CVector dist = Source - target;

	if(ResetStatics)
		Source = target + dist*(CA_MAX_DISTANCE + 1.0f);

	float distMag = Magnitude2D(dist);
	if(distMag < 0.001f){
		// This probably shouldn't happen. reset view
		CVector fwd = CamTargetEntity->matrix.GetForward();
		fwd.z = 0.0f;
		fwd.Normalise();
		Source = target - fwd*CA_MAX_DISTANCE;
		dist = Source - target;
		distMag = Magnitude2D(dist);
	}

	if(distMag > CA_MAX_DISTANCE){
		Source.x = target.x + dist.x/distMag * CA_MAX_DISTANCE;
		Source.y = target.y + dist.y/distMag * CA_MAX_DISTANCE;
	}else if(distMag < CA_MIN_DISTANCE){
		Source.x = target.x + dist.x/distMag * CA_MIN_DISTANCE;
		Source.y = target.y + dist.y/distMag * CA_MIN_DISTANCE;
	}
}
#endif

#if 0
WRAPPER void CCam::WorkOutCamHeight(const CVector &target, float targetOrientation, float targetHeight) { EAXJMP(0x466650); }
#else
void
CCam::WorkOutCamHeight(const CVector &target, float targetOrientation, float targetHeight)
{
	static float LastTargetAlphaWithCollisionOn = 0.0f;
	static float LastTopAlphaSpeed = 0.0f;
	static float LastAlphaSpeedStep = 0.0f;
	static bool PreviousNearCheckNearClipSmall = false;

	bool camClear = true;
	float modeAlpha = 0.0f;

	if(ResetStatics){
		LastTargetAlphaWithCollisionOn = 0.0f;
		LastTopAlphaSpeed = 0.0f;
		LastAlphaSpeedStep = 0.0f;
		PreviousNearCheckNearClipSmall = false;
	}

	float topAlphaSpeed = 0.15f;
	float alphaSpeedStep = 0.015f;

	float zoomvalue = TheCamera.CarZoomValueSmooth;
	if(zoomvalue < 0.1f)
		zoomvalue = 0.1f;
	if(TheCamera.CarZoomIndicator == 1.0f)
		modeAlpha = CGeneral::GetATanOfXY(23.0f, zoomvalue);	// near
	else if(TheCamera.CarZoomIndicator == 2.0f)
		modeAlpha = CGeneral::GetATanOfXY(10.8f, zoomvalue);	// mid
	else if(TheCamera.CarZoomIndicator == 3.0f)
		modeAlpha = CGeneral::GetATanOfXY(7.0f, zoomvalue);	// far


	float dist = Magnitude2D(Source - target);
	if(m_bCollisionChecksOn){	// there's another variable (on PC) but it's uninitialied o_O
		CVector fwd = CamTargetEntity->matrix.GetForward();
		float carAlpha = CGeneral::GetATanOfXY(Magnitude2D(fwd), fwd.z);
		// this shouldn't be necessary....
		while(carAlpha > PI) carAlpha -= 2*PI;
		while(carAlpha < -PI) carAlpha += 2*PI;

		while(Beta > PI) Beta -= 2*PI;
		while(Beta < -PI) Beta += 2*PI;

		float deltaBeta = Beta - targetOrientation;
		while(deltaBeta > PI) deltaBeta -= 2*PI;
		while(deltaBeta < -PI) deltaBeta += 2*PI;

		float behindCar = cos(deltaBeta);	// 1 if behind car, 0 if side, -1 if in front
		carAlpha = -carAlpha * behindCar;
		if(carAlpha < -0.01f)
			carAlpha = -0.01f;

		float deltaAlpha = carAlpha - Alpha;
		while(deltaAlpha > PI) deltaAlpha -= 2*PI;
		while(deltaAlpha < -PI) deltaAlpha += 2*PI;
		// What's this?? wouldn't it make more sense to clamp?
		float anglelimit = DEGTORAD(1.8f);
		if(deltaAlpha < -anglelimit)
			deltaAlpha += anglelimit;
		else if(deltaAlpha > anglelimit)
			deltaAlpha -= anglelimit;
		else
			deltaAlpha = 0.0f;

		// Now the collision

		float targetAlpha = 0.0f;
		bool foundRoofCenter = false;
		bool foundRoofSide1 = false;
		bool foundRoofSide2 = false;
		bool foundCamRoof = false;
		bool foundCamGround = false;
		float camroof = 0.0f;
		float carBottom = target.z - targetHeight/2.0f;

		// Check car center
		float roofz = CWorld__FindRoofZFor3DCoord(target.x, target.y, carBottom, &foundRoofCenter);

		// Check sides of the car
		fwd = CamTargetEntity->matrix.GetForward();	// we actually still have that...
		fwd.Normalise();	// shouldn't be necessary
		float carSideAngle = CGeneral::GetATanOfXY(fwd.x, fwd.y) + PI/2.0f;
		float sidex = 2.5f * cos(carSideAngle);
		float sidey = 2.5f * sin(carSideAngle);
		CWorld__FindRoofZFor3DCoord(target.x + sidex, target.y + sidey, carBottom, &foundRoofSide1);
		CWorld__FindRoofZFor3DCoord(target.x - sidex, target.y - sidey, carBottom, &foundRoofSide2);

		// Now find out at what height we'd like to place the camera
		float camground = CWorld__FindGroundZFor3DCoord(Source.x, Source.y, target.z + dist*sin(Alpha + modeAlpha) + m_fCloseInCarHeightOffset, &foundCamGround);
		float camTargetZ = 0.0f;
		if(foundCamGround){
			// This is the normal case
			camroof = CWorld__FindRoofZFor3DCoord(Source.x, Source.y, camground + targetHeight, &foundCamRoof);
			camTargetZ = camground + targetHeight*1.5f + 0.1f;
		}else{
			foundCamRoof = false;
			camTargetZ = target.z;
		}

		if(foundRoofCenter && !foundCamRoof && (foundRoofSide1 || foundRoofSide2)){
			// Car is under something but camera isn't
			// This seems weird...
			targetAlpha = CGeneral::GetATanOfXY(CA_MAX_DISTANCE, roofz - camTargetZ - 1.5f);
			camClear = false;
		}
		if(foundCamRoof){
			// Camera is under something
			float roof = foundRoofCenter ? MIN(camroof, roofz) : camroof;
			// Same weirdness again?
			targetAlpha = CGeneral::GetATanOfXY(CA_MAX_DISTANCE, roof - camTargetZ - 1.5f);
			camClear = false;
		}
		while(targetAlpha > PI) targetAlpha -= 2*PI;
		while(targetAlpha < -PI) targetAlpha += 2*PI;
		if(targetAlpha < DEGTORAD(-7.0f))
			targetAlpha = DEGTORAD(-7.0f);

		// huh?
		if(targetAlpha > modeAlpha)
			camClear = true;
		// Camera is contrained by collision in some way
		PreviousNearCheckNearClipSmall = false;
		if(!camClear){
			PreviousNearCheckNearClipSmall = true;
			RwCameraSetNearClipPlane(Scene.camera, 0.9f);

			deltaAlpha = targetAlpha - (Alpha + modeAlpha);
			while(deltaAlpha > PI) deltaAlpha -= 2*PI;
			while(deltaAlpha < -PI) deltaAlpha += 2*PI;

			topAlphaSpeed = 0.3f;
			alphaSpeedStep = 0.03f;
		}

		// Now do things if camClear...but what is that anyway?
		float camz = target.z + dist*sin(Alpha + deltaAlpha + modeAlpha) + m_fCloseInCarHeightOffset;
		bool foundGround, foundRoof;
		float camground2 = CWorld__FindGroundZFor3DCoord(Source.x, Source.y, camz, &foundGround);
		if(foundGround){
			if(camClear)
				if(camz - camground2 < 1.5f){
					PreviousNearCheckNearClipSmall = true;
					RwCameraSetNearClipPlane(Scene.camera, 0.9f);

					float a;
					if(dist == 0.0f || camground2 + 1.5f - target.z == 0.0f)
						a = Alpha;
					else
						a = CGeneral::GetATanOfXY(dist, camground2 + 1.5f - target.z);
					while(a > PI) a -= 2*PI;
					while(a < -PI) a += 2*PI;
					deltaAlpha = a - Alpha;
				}
		}else{
			if(camClear){
				float camroof2 = CWorld__FindRoofZFor3DCoord(Source.x, Source.y, camz, &foundRoof);
				if(foundRoof && camz - camroof2 < 1.5f){
					PreviousNearCheckNearClipSmall = true;
					RwCameraSetNearClipPlane(Scene.camera, 0.9f);

					if(camroof2 > target.z + 3.5f)
						camroof2 = target.z + 3.5f;

					float a;
					if(dist == 0.0f || camroof2 + 1.5f - target.z == 0.0f)
						a = Alpha;
					else
						a = CGeneral::GetATanOfXY(dist, camroof2 + 1.5f - target.z);
					while(a > PI) a -= 2*PI;
					while(a < -PI) a += 2*PI;
					deltaAlpha = a - Alpha;
				}
			}
		}

		LastTargetAlphaWithCollisionOn = deltaAlpha + Alpha;
		LastTopAlphaSpeed = topAlphaSpeed;
		LastAlphaSpeedStep = alphaSpeedStep;
	}else{
		if(PreviousNearCheckNearClipSmall)
			RwCameraSetNearClipPlane(Scene.camera, 0.9f);
	}

	WellBufferMe(LastTargetAlphaWithCollisionOn, &Alpha, &AlphaSpeed, LastTopAlphaSpeed, LastAlphaSpeedStep, true);

	Source.z = target.z + sin(Alpha + modeAlpha)*dist + m_fCloseInCarHeightOffset;
}
#endif

// Move up camera when camera clips into a vehicle
#if 0
WRAPPER void CCam::FixCamWhenObscuredByVehicle(const CVector &target) { EAXJMP(0x457A80); }
#else
void
CCam::FixCamWhenObscuredByVehicle(const CVector &target)
{
	// WTF? is this never reset?
	static float HeightFixerCarsObscuring = 0.0f;
	static float HeightFixerCarsObscuringSpeed = 0.0f;
	CColPoint colpoint;
	CEntity *ent;

	float ztarget = 0.0f;
	if(CWorld__ProcessLineOfSight(target, Source, colpoint, ent, false, true, false, false, false, false, false)){
		CBaseModelInfo *mi = CModelInfo::ms_modelInfoPtrs[ent->nModelIndex];
		ztarget = mi->colModel->boundingBox.max.z + 1.0f + target.z - Source.z;
		if(ztarget < 0.0f)
			ztarget = 0.0f;
	}
	WellBufferMe(ztarget, &HeightFixerCarsObscuring, &HeightFixerCarsObscuringSpeed, 0.2f, 0.025f, false);
	Source.z += HeightFixerCarsObscuring;
}
#endif

// In vehicle cam
void
CCam::Process_Cam_On_A_String(const CVector &ThisCamsTarget, float TargetOrientation, float SpeedVar, float SpeedVarDesired)
{
	if((CamTargetEntity->bfTypeStatus & 7) != 2)
		return;

	FOV = ::FOV;

	if(ResetStatics){
		AlphaSpeed = 0.0f;
		if(TheCamera.m_bIdleOn)
			TheCamera.m_uiTimeWeEnteredIdle = CTimer::m_snTimeInMilliseconds;
	}

	CBaseModelInfo *mi = CModelInfo::ms_modelInfoPtrs[CamTargetEntity->nModelIndex];
	CVector dimensions = mi->colModel->boundingBox.max - mi->colModel->boundingBox.min;
	float distval = Magnitude2D(dimensions);

	CVector target = ThisCamsTarget;
	target.z += dimensions.z - 0.1f;
	Beta = CGeneral::GetATanOfXY(target.x - Source.x, target.y - Source.y);
	while(Alpha > PI) Alpha -= 2*PI;
	while(Alpha < -PI) Alpha += 2*PI;
	while(Beta > PI) Beta -= 2*PI;
	while(Beta < -PI) Beta += 2*PI;

	m_fDistanceBeforeChanges = Magnitude2D(Source - target);

	Cam_On_A_String_Unobscured(target, distval);
	WorkOutCamHeight(target, TargetOrientation, dimensions.z);
	RotCamIfInFrontCar(target, TargetOrientation);
	FixCamIfObscured(target, dimensions.z, TargetOrientation);
	FixCamWhenObscuredByVehicle(target);

	m_cvecTargetCoorsForFudgeInter = target;
	Front = target - Source;
	Front.Normalise();
	GetVectorsReadyForRW();
	ResetStatics = false;
}

void
hook(void)
{
	InjectHook(0x459A54, &CCam::Process_Cam_On_A_String);
}
