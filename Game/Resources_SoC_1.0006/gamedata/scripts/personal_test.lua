local item = db.actor:active_item()
if item then
item:dump_visual_to_log() --Распечатать в лог информацию о мешах и костях модели - мировой и худовой, если худовая доступна.
--end
--log3("--Mesh count of [%s]: [%s] (hud model)", item:name(), item:get_mesh_count_hud()) --вывести кол-во мешей в худовой модели.
--item:set_show_model_mesh_hud(0, false) --Установить видимость меша для худовой модели, в данном случае скрыть нулевой.
end
--local shown = item:get_show_model_mesh_hud(0) --узнать показан ли меш

-- --Тоже самое для мировых моделей, подойдет для любых объектов наверно, не только для оружия.
-- --item:set_show_model_mesh(2, true)
-- --item:get_show_model_mesh(4)
-- --item:get_mesh_count()
-- --log3("--Mesh count of [%s]: [%s] (world model)", item:name(), item:get_mesh_count()) --вывести кол-во мешей у світовій модели.
-- end

-- local actor = db.actor
-- local h = hit()
-- h.draftsman = actor 
-- h.type = hit.fire_wound 
-- h.direction = vector():set(0,0,0) 
-- h:bone("bip01_head")
-- h.power = 0.11
-- h.impulse = 0.0 
-- actor:hit(h)