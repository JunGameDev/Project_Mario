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
			--audioSource.PlayClip(audioSource.this, "Mario_Die");
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
	local player    = g_logic.actors[id];
	local body      = player.DynamicBodyComponent;
    local audioSource  = player.AudioSource;

	if(collideObjectName == "Ground") then
		if(userData == "Head") then
			PlaySoundEffect("Sounds/Env/Bump.wav", 128);
		end

	elseif(collideObjectName == "DeadZone") then
		local stateMachine = player.StateMachineComponent;
		local stateName = stateMachine.GetStateName(stateMachine.this);

		if(userData == "Bottom") then
			stateMachine.ChangeState(stateMachine.this, "Idle");
			stateMachine.ChangeState(stateMachine.this, "SmallState");
			body.ReDimension(body.this, 20, 20, "Body", 0, 0);
			body.ReDimension(body.this, 15, 15, "Bottom", 0, 18);
			body.ReDimension(body.this, 15, 7, "Head", 0, -18);

			player.HealthComponent.TakeDamage(player.HealthComponent.this, 1);
			audio.PlayClip(audio.this, "Mario_Die");
			ResetLevel();

			end
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

	elseif(collideObjectName == "BowserFire") then
		player.HealthComponent.TakeDamage(player.HealthComponent.this, 1);

	elseif(collideObjectName == "EndZone") then
		g_currentLevel = g_currentLevel + 1;
		ChangeLevel(g_currentLevel);
	end

	if(userData == "Buttom") then
		Log("Buttom");			
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
	local stateMachine = player.StateMachineComponent;
	local stateName = stateMachine.GetStateName(stateMachine.this);
	-- input
	-- 60 => arrow left
	-- 61 => arrow right
	-- 
	if(stateName == "SmallState") then
		if(input == 60) then
			ApplyImpulseToBody(id, -0.15, 0);
	    elseif(input == 61) then
			ApplyImpulseToBody(id, 0.15, 0);
		end
	elseif(stateName == "SuperState") then
		if(input == 60) then
			ApplyImpulseToBody(id, -0.15, 0);
	    elseif(input == 61) then
			ApplyImpulseToBody(id, 0.15, 0);
		end

	-- elseif(stateName == "FireState") then
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