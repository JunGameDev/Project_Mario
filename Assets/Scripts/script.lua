-- local scriptPath = "..\\Assets\\Scripts\\"

-- dofile(scriptPath .. "Mario.lua")	-- Script for mario

g_playerId = g_logic.CreateActor("Actors/Mario.xml", "Mario");
g_currentLevel = 1;

g_levels = 
{
	{
		path   = "Levels/SuperMario_Stage_1.tmx",
		start  = { x = 29.00 * 3, y = 184.00 * 3},
		camera = { x = 384, y = 360 },
		music  = "Sounds/Theme/Overworld.mp3"
	};

	{
		path   = "Levels/SuperMario_Stage_2.tmx",
		start  = { x = 54.00 * 3, y = 121.00 * 3},
		camera = { x = 384, y = 600 },
		music  = "Sounds/Theme/Underground.mp3"
	};

	{
		path   = "Levels/SuperMario_Stage_3.tmx",
		start  = { x = 19.00 * 3, y = 281.00 * 3},
		camera = { x = 384, y = 600 },
		music  = "Sounds/Theme/Overworld.mp3"
	};

	{
		path   = "Levels/SuperMario_Stage_5.tmx",
		start  = { x = 18.00 * 3, y = 193.00 * 3},
		camera = { x = 384, y = 600 },
		music  = "Sounds/Theme/Castle.mp3"
	};
};

-- Recursive Print in Lua (structure, limit, indent)
function RecursivePrint(structure, limit, indent) 
	limit = (limit) or 100;
	indent = i or "";	-- default item limit, indent string
	
	if (limit < 1) then
		print("ERROR: Item limit reached.");
		return limit - 1 
	end;
	
	local ts = type(structure);
    
	if (ts ~= "table") then
		print(indent,ts,structure);
		return limit - 1
	end
    
	print (indent,ts); -- print "table"
    
	for key, value in pairs(s) do  -- print "[key] value"
		limit = rPrint(value, limit, indent.."\t["..tostring(k).."]");
		if (limit < 0) then 
			break 
		end
	end
	return limit
end	

kFramerate = 60;

function AddActorIdToTable(id)
    g_logic.SetActorsToScript(id);
end

function MoveActor(id, xDelta, yDelta)
	local dynamicBody = g_logic.actors[id].DynamicBodyComponent;
	dynamicBody.Move(dynamicBody.this, xDelta, yDelta);
end

function TeleportActor(id, xPos, yPos)
    local transform = g_logic.actors[id].TransformComponent;
    transform.SetPosition(transform.this, xPos, yPos);
end

function MoveCamera(xValue, yValue)
	local camera = g_logic.Camera;
	camera.Move(xValue, yValue);
end

function FixCameraToTransform(transform)
	local camera	= g_logic.Camera;
	camera.SetPosition(transform.Position.x, transform.Position.y);
end

function SetCameraPos(x, y)
	g_logic.Camera.SetPosition(x, y);
end

function FixCameraToActor(actorId)
	local transform = g_logic.actors[actorId].TransformComponent;

	local x = transform.Position.GetX(transform.this);
	local y = transform.Position.GetY(transform.this);
	Log("[FixCameraToActor] X: " .. x .. " Y: " .. y, true);
	
	g_logic.Camera.SetPosition(x, y);
end

function DestroyActor(id)
	g_logic.DestroyActor(id);
end

function DestroyView(id)
	g_logic.DestroyView(id);
end

function CreateProjectile(path)
    local projectileId = g_logic.CreateActor(path);
end

function MoveCamera(xPos, yPos)
	g_logic.Camera.Move(xPos, yPos);
	-- Log("MoveCamera() was called from script.lua", true);
end

function PlaySoundEffect(name, volume)
	g_logic.PlaySoundEffect(name, volume);
	Log(name, true);
end

function PlayMusic(name, volume)
	Log(name, true);
    g_logic.PlayMusic(name, volume);
end

function StopMusic()
    g_logic.StopMusic();
end

function MuteMusic()
	g_logic.MuteMusic();
end

function ResumeMusic()
	g_logic.ResumeMusic();
end

function ChangeMusic(name, volume)
    g_logic.ChangeMusic(name, volume);
end

function SpawnItem(file, name, posX, posY)
	g_logic.SpawnItem(file, name, posX + 15, posY - 15);
end


function Log(message, newLine)
	local log = g_logic.Logging;
	log.Log(log.this, message, newLine);
end

function UpdateCamera()
	local player = g_logic.actors[g_playerId];
	local transform = player.TransformComponent;
	local renderingX = g_logic.Camera.GetX();

	if(transform.Position.GetX(transform.this) - renderingX > g_logic.Camera.GetCenterX()) then
		SetCameraPos(transform.Position.GetX(transform.this), g_levels[g_currentLevel].camera.y);
	end
end

function PlayerOnCollisionEnter(id, collideId, collideObjectName, userData)
	local player    = g_logic.actors[g_playerId];
	local body      = player.DynamicBodyComponent;
    local audioSource  = player.AudioSource;
	
	if(collideObjectName == "Mushroom" or collideObjectName == "Flower")  then
		audioSource.PlayClip(audioSource.this, "PowerUp");
	elseif(collideObjectName == "ExtraLife") then
		audioSource.PlayClip(audioSource.this, "1-up");
		player.HealthComponent.GainPoint(player.HealthComponent.this, 1);

	elseif(collideObjectName == "Gumba") then
		local stateMachine = player.StateMachineComponent;
		local stateName = stateMachine.GetStateName(stateMachine.this);

		if(stateName == "SmallState") then
			audioSource.PlayClip(audioSource.this, "Mario_Die");
			ResetLevel();
			player.HealthComponent.TakeDamage(player.HealthComponent.this, 1);

		elseif(stateName == "SuperState") then
			ApplyImpulseToBody(id, 0, -12);
			stateMachine.ChangeState(stateMachine.this, "Idle");
			stateMachine.ChangeState(stateMachine.this, "SmallState");
			body.ReDimension(body.this, 20, 20, "Body", 0, 0);
			body.ReDimension(body.this, 15, 15, "Bottom", 0, 18);
			body.ReDimension(body.this, 15, 7, "Head", 0, -18);
			PlaySoundEffect("Sounds/Env/PipeAndPowerDown.wav", 128);
		end
	end
end

function PlayerOnTriggerEnter(id, collideId, collideObjectName, userData)
	local player    = g_logic.actors[g_playerId];
	local body      = player.DynamicBodyComponent;
    local audioSource  = player.AudioSource;

	if(collideObjectName == "Ground") then
		if(userData == "Head") then
			PlaySoundEffect("Sounds/Env/Bump.wav", 128);
		end

	elseif(collideObjectName == "DeadZone") then
		local stateMachine = player.StateMachineComponent;
		local stateName = stateMachine.GetStateName(stateMachine.this);

		stateMachine.ChangeState(stateMachine.this, "Idle");
		stateMachine.ChangeState(stateMachine.this, "SmallState");
		body.ReDimension(body.this, 20, 20, "Body", 0, 0);
		body.ReDimension(body.this, 15, 15, "Bottom", 0, 18);
		body.ReDimension(body.this, 15, 7, "Head", 0, -18);
		player.HealthComponent.TakeDamage(player.HealthComponent.this, 1);
		audioSource.PlayClip(audioSource.this, "Mario_Die");
		ResetLevel();

	elseif(collideObjectName == "GameClear") then
		StopMusic();
		player.View.SetControlable(player.View.this, false);
		PlaySoundEffect("Sounds/Env/GameClear.mp3", 64);
		return;
	
	elseif(collideObjectName == "Gumba") then
		if(userData == "Bottom") then
			DestroyView(collideId);
			DestroyActor(collideId);
			ApplyImpulseToBody(id, 0, -12);
			PlaySoundEffect("Sounds/Mario/Kick.wav", 128);
		end

	elseif(collideObjectName == "BowserFire" 
			or string.find(collideObjectName, "FireChain")) then
		player.HealthComponent.TakeDamage(player.HealthComponent.this, 1);

	elseif(collideObjectName == "EndZone") then
		g_currentLevel = g_currentLevel + 1;
		ChangeLevel(g_currentLevel);
	end

	if(userData == "Buttom") then
		if(collideObjectName == "Wall" or collideObjectName == "Ground" or collideObjectName == "Crate" or collideObjectName == "Block" or collideObjectName == "Boundary") then
			Log(g_jumpingTimeDelta);			
			Log(g_jumpingTimeDelta);
		end
	end

	player.View.SetGrounded(player.View.this, isGround);
end

function PlayerOnTriggerExit(id, collideObjectName, userData)
end

function FireBallOnCollisionEnter(id, collideId, collideObjectName, userData, headingRight)
	local fireBall     = g_logic.actors[id];

	if(collideObjectName == "Ground" or collideObjectName == "Boundary") then
		if(headingRight == true) then
			ApplyImpulseToBody(id, 1.5, -1.6);
		else
			ApplyImpulseToBody(id, -1.5, -1.6);
		end
	elseif(collideObjectName == "Wall") then
		DestroyView(id);
		DestroyActor(id);
	elseif(collideObjectName == "Gumba") then
		DestroyView(collideId);
		DestroyActor(collideId);

		DestroyView(id);
		DestroyActor(id);

		PlaySoundEffect("Sounds/Mario/Kick.wav", 128);
	end
end

function MovePlayer(input, id)
	local player = g_logic.actors[id];
	local playerPosX = player.TransformComponent.Position.GetX(player.TransformComponent.this);
	local camera = g_logic.Camera;

	-- input
	-- 60 => arrow left
	-- 61 => arrow right

	if(input == 60) then
		if(playerPosX - camera.GetX() <= camera.GetCenterX() - (camera.GetScreenWidth() / 2) + 14) then
			SetLinearXVelToBody(id, 0);
			return;
		end
			ApplyImpulseToBody(id, -0.15, 0);
	elseif(input == 61) then
		ApplyImpulseToBody(id, 0.15, 0);
	end
end

function JumpPlayer(id, isGround, jumpDelta)
	local player = g_logic.actors[id];
	local stateMachine = player.StateMachineComponent;
	local stateName = stateMachine.GetStateName(stateMachine.this);
	local audioSource  = player.AudioSource;

	if(isGround == false) then
		if(jumpDelta < 0.11) then
			ApplyImpulseToBody(id, 0, -2.6);
		elseif(jumpDelta > 0.13 and jumpDelta < 0.34) then
			ApplyImpulseToBody(id, 0, -0.25);
		end
	end
end

function ApplyImpulseToBody(id, xVel, yVel)
	local player = g_logic.actors[id];
	local body = player.DynamicBodyComponent;
	body.ApplyLinearImpulse(body.this, xVel, yVel);
end

function SetLinearXVelToBody(id, vel)
	local player = g_logic.actors[id];
	local body = player.DynamicBodyComponent;
	body.SetLinearVelocityX(body.this, vel);
end

function PickUpItem(name, id)
	local player       = g_logic.actors[g_playerId];
	local stateMachine = player.StateMachineComponent;
	local audioSource  = player.AudioSource;
	local body      = player.DynamicBodyComponent;

	if (name == "Mushroom") then
		stateMachine.ChangeState(stateMachine.this, "Idle");
		stateMachine.ChangeState(stateMachine.this, "SuperState");
		body.ReDimension(body.this, 20, 40, "Body", 0, 0);
		body.ReDimension(body.this, 15, 35, "Bottom", 0, 18);
		body.ReDimension(body.this, 15, 14, "Head", 0, -38);
	end

	Log("Item: " .. name .. " has been picked up by player", true);
end

function PlayerHandleMouseDown(id, btn, mouseX, mouseY)
	-- 0 => Left click
	-- 1 => Right click
end

function PlayerHandleMousePressed(id, btn, mouseX, mouseY)
	-- 0 => Left click
	-- 1 => Right click

	local player = g_logic.actors[id];
	local view = player.View;

	if(btn == 0) then
		view.SpawnFireBall(id, mouseX, mouseY);
	end
end

function CheckPlayerHp()
	local player = g_logic.actors[g_playerId];
	local audio = player.AudioSource; 

	local hp = player.HealthComponent.GetHP(player.HealthComponent.this);

	if (hp <= 0) then
		StopMusic();
		PlaySoundEffect("Sounds/Env/GameOver.wav", 128);
		player.View.SetControlable(player.View.this, false);
	end
end
function InitializeGame()
	g_logic.CreatePlayerView(g_playerId);

	Log(g_levels[1].path .." start: " .. g_levels[1].start.x .. ", " .. g_levels[1].start.y, true);
	Log(g_levels[2].path .." start: " .. g_levels[2].start.x .. ", " .. g_levels[2].start.y, true);
	Log(g_levels[3].path .." start: " .. g_levels[3].start.x .. ", " .. g_levels[3].start.y, true);
	Log(g_levels[4].path .." start: " .. g_levels[4].start.x .. ", " .. g_levels[4].start.y, true);

	g_logic.LoadLevel(g_levels[g_currentLevel].path, g_levels[g_currentLevel].start.x, g_levels[g_currentLevel].start.y, g_levels[g_currentLevel].camera.x, g_levels[g_currentLevel].camera.y);	
	PlayMusic(g_levels[g_currentLevel].music, 128);
end

function ChangeLevel(index) 
	g_logic.LoadLevel(g_levels[index].path, g_levels[index].start.x, g_levels[index].start.y, g_levels[index].camera.x, g_levels[index].camera.y);
	PlayMusic(g_levels[index].music, 128);
	g_currentLevel = index;
end

function ResetLevel()
	ChangeLevel(g_currentLevel);
end

function StartNewGame()
	g_currentLevel = 1;
	local player    = g_logic.actors[g_playerId];
	player.HealthComponent.SetHP(player.HealthComponent.this, 5);
	player.View.SetControlable(player.View.this, true);
	ChangeLevel(g_currentLevel);
end

InitializeGame();