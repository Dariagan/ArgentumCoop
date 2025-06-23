struct game{
    tilename_nid_mapping: Vec<StringName>,

}


 /*
    //la save debe generar propias ids unicas
    //esto debería ser al cargar la save, con las tiles de la save serializadas previamente
    //nunca perder la id de las tiles borradas por si se instala el mod de vuelta
    fn merge_tiles_data(&mut self, inp_arr: VariantArray) -> bool {
        
        match exceeds_tile_limit(&inp_arr) {
            Ok(()) => (),
            Err(()) => {
                godot_print!("no caben las tiles"); 
                return false;
            }
        }
        if self.tiles_data.is_empty() {
            self.tile_nid_mapping.reserve_exact(inp_arr.len());
            self.tile_nid_mapping.extend(
            inp_arr
                .iter_shared()
                .collect()
            );
        } else {
            let keys_to_add: Vec<_> = inp_dict
                .keys_array()
                .iter_shared()
                .filter_map(|key| {
                    let key_str = StringName::from_variant(&key);
                    if !self.name_nid_mapping.contains(&key_str) {
                        Some(key_str)
                    } else {
                        None
                    }
                })
                .collect();

            self.name_nid_mapping.extend(keys_to_add);
        }
        self.tiles_data = inp_dict;
        true
    }*/