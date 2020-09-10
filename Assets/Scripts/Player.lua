local path = "..\\Assets\\Scripts\\"
dofile(path .. "Character.lua")

-- Player class definition 
Player = Character()

function Player:Create(x, y)
    print("--- Player:Create ---")

    self.m_x = x
    self.m_y = y
    self.m_tag = "Player"
    self.m_hasEscapeKey = false
    self.m_hasTreasureKey = false
    self.m_openedTreasure = false
    self.m_hasFin = false
    self.m_canSwim = false

    newObj = {}
    self.__index = self
    return setmetatable(newObj, self)
end

Player.call = { __call = Player.Create }
setmetatable(Player, Player.call)

function Player:Render()
    io.write('P ')
end

function Player:HandleInput(input)
    if (input == 'w') then
        self.m_y = self.m_y - 1
    elseif (input == 'a') then
        self.m_x = self.m_x - 1
    elseif (input == 's') then
        self.m_y = self.m_y + 1
    elseif (input == 'd') then
        self.m_x = self.m_x + 1
    elseif(input == 'swim') then
        if(self.m_hasFin == true) then
            self.m_canSwim = true
        end
    else 
        if(input ~= 'pick' and input ~='open' and input ~= 'unlock') then
            print("That was wrong input!")
        end
    end            
end

function Player:GetX()
    return self.m_x
end

function Player:GetY()
    return self.m_y
end

function Player:RootTreasure(value)
    self.m_openedTreasure = value
end

function Player:CheckRootTreasure()
    return self.m_openedTreasure
end

function Player:RootEscapeKey(value)
    self.m_hasEscapeKey = value
end

function Player:RootTreasureKey(value)
    self.m_hasTreasureKey = value
end

function Player:RootFin(value)
    self.m_hasFin = value
end

function Player:HasFin()
    return self.m_hasFin
end

function Player:HasTreasureKey()
    return self.m_hasTreasureKey
end

function Player:CanSwim(value)
    return self.m_canSwim
end



function Player:SetPosition(x, y)
    self.m_x = x
    self.m_y = y
end