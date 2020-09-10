Character = {}                                    
Character.mt = { __index = Character}

function Character:Create(x, y) 
    local obj = {
        m_x = 0 or x,
        m_y = 0 or y,
    }
    obj.Character = obj or {}
    setmetatable(obj, Character.mt)
    return obj
end

Character.call = { __call = Character.Create }
setmetatable(Character, Character.call)

function Character:Update() 
    print("--- Character:Update ---")
end

function Character:Render()
    io.write("C ")
end

function Character:GetX()
    return self.m_x
end

function Character:GetY()
    return self.m_y
end

function Character:GetPosition()
    return self.m_x, self.m_y
end

