-- Character = {}                                    
-- Character.mt = { __index = Character}

-- function Character:Create(x, y) 
--     local obj = {
--         m_x = 0 or x,
--         m_y = 0 or y,
--     }
--     obj.Character = obj or {}
--     setmetatable(obj, Character.mt)
--     return obj
-- end

-- Character.call = { __call = Character.Create }
-- setmetatable(Character, Character.call)

-- function Character:Update() 
--     print("--- Character:Update ---")
-- end

-- function Character:Render()
--     io.write("C ")
-- end

-- function Character:GetX()
--     return self.m_x
-- end

-- function Character:GetY()
--     return self.m_y
-- end

-- function Character:GetPosition()
--     return self.m_x, self.m_y
-- end

-- -- Player class definition 
-- Player = Character()

-- function Player:Create(x, y)
--     print("--- Player:Create ---")

--     self.m_x = x
--     self.m_y = y
--     self.m_tag = "Player"
--     self.m_hasEscapeKey = false
--     self.m_hasTreasureKey = false
--     self.m_openedTreasure = false
--     self.m_hasFin = false
--     self.m_canSwim = false

--     newObj = {}
--     self.__index = self
--     return setmetatable(newObj, self)
-- end

-- Player.call = { __call = Player.Create }
-- setmetatable(Player, Player.call)

-- function Player:Render()
--     io.write('P ')
-- end

-- function Player:HandleInput(input)
--     if (input == 'w') then
--         self.m_y = self.m_y - 1
--     elseif (input == 'a') then
--         self.m_x = self.m_x - 1
--     elseif (input == 's') then
--         self.m_y = self.m_y + 1
--     elseif (input == 'd') then
--         self.m_x = self.m_x + 1
--     elseif(input == 'swim') then
--         if(self.m_hasFin == true) then
--             self.m_canSwim = true
--         end
--     else
--         if(input ~= 'pick' and input ~='open' and input ~= 'unlock') then
--             print("That was wrong input!")
--         end
--     end            
-- end

-- function Player:GetX()
--     return self.m_x
-- end

-- function Player:GetY()
--     return self.m_y
-- end

-- function Player:RootTreasure(value)
--     self.m_openedTreasure = value
-- end

-- function Player:CheckRootTreasure()
--     return self.m_openedTreasure
-- end

-- function Player:RootEscapeKey(value)
--     self.m_hasEscapeKey = value
-- end

-- function Player:RootTreasureKey(value)
--     self.m_hasTreasureKey = value
-- end

-- function Player:RootFin(value)
--     self.m_hasFin = value
-- end

-- function Player:HasFin()
--     return self.m_hasFin
-- end

-- function Player:HasTreasureKey()
--     return self.m_hasTreasureKey
-- end

-- function Player:CanSwim(value)
--     return self.m_canSwim
-- end



-- function Player:SetPosition(x, y)
--     self.m_x = x
--     self.m_y = y
-- end

-- Door class definition 
Door = {}                                    
Door.mt = { __index = Door}

function Door:Create(x, y, sX, sY)
    local obj = {
        m_x = x,
        m_y = y,
        m_spawnX = sX,
        m_spawnY = sY,
        m_tag = "Door",
        m_mapConnectedMap = nil
    }
    obj.Door = obj or {}
    setmetatable(obj, Door.mt)
    return obj
end

Door.call = { __call = Door.Create }
setmetatable(Door, Door.call)

function Door:GetX()
    return self.m_x
end

function Door:GetY()
    return self.m_y
end

function Door:GetSpawnX()
    return self.m_spawnX
end

function Door:GetSpawnY()
    return self.m_spawnY
end

function Door:SetConnectedMap(obj)
    self.m_mapConnectedMap = obj
end

function Door:GetConnectedMap()
    return self.m_mapConnectedMap
end

-- Key class definition 
Key = {}                                    
Key.mt = { __index = Key}

function Key:Create(x, y)
    local obj = {
        m_x = x,
        m_y = y,
        m_tag = "Key"
    }
    obj.Key = obj or {}
    setmetatable(obj, Key.mt)
    return obj
end

Key.call = { __call = Key.Create }
setmetatable(Key, Key.call)

function Key:Render()
    io.write('K ')
end

function Key:SetTag(str)
    self.m_tag = str
end

function Key:GetX()
    return self.m_x
end

function Key:GetY()
    return self.m_y
end

function Key:GetTag()
    return self.m_tag
end

-- TreasureBox definition
Treasure = {}                                    
Treasure.mt = { __index = Treasure }

function Treasure:Create(x, y)
    local obj = {
        m_x = x,
        m_y = y,
        m_tag = "Treasure"
    }
    obj.Treasure = obj or {}
    setmetatable(obj, Treasure.mt)
    return obj
end

Treasure.call = { __call = Treasure.Create }
setmetatable(Treasure, Treasure.call)

function Treasure:Render()
    io.write('T ')
end

function Treasure:GetX()
    return self.m_x
end

function Treasure:GetY()
    return self.m_y
end

function Treasure:GetTag()
    return self.m_tag
end

-- TreasureBox definition
SwimmingSuit = {}                                    
SwimmingSuit.mt = { __index = SwimmingSuit }

function SwimmingSuit:Create(x, y)
    local obj = {
        m_x = x,
        m_y = y,
        m_tag = "SwimmingSuit"
    }

    obj.SwimmingSuit = obj or {}
    setmetatable(obj, SwimmingSuit.mt)
    return obj
end

SwimmingSuit.call = { __call = SwimmingSuit.Create }
setmetatable(SwimmingSuit, SwimmingSuit.call)

function SwimmingSuit:Render()
    io.write('F ')
end

function SwimmingSuit:GetX()
    return self.m_x
end

function SwimmingSuit:GetY()
    return self.m_y
end

function SwimmingSuit:GetTag()
    return self.m_tag
end
