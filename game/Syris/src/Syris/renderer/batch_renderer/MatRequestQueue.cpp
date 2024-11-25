namespace Syris{
    /**************** OPENGL MATERIAL QUEUE IMPLEMENTATION ********************/
    /*
    template<typename T>
    void RequestQueue<T>::add_request(MaterialRequest<T>& request){
        
        if (m_entity_to_index.find(request.entity) != m_entity_to_index.end()){
            CORE_ERROR("Adding entity twice");
            exit(1);
        }
        m_entity_to_index[request.entity] = m_request_data.size();
        m_index_to_entity[m_request_data.size()] = request.entity;
        m_request_data.push_back(request.data);
    }

    template<typename T>
    bool RequestQueue<T>::remove_request(entt::entity entity){
        uint64_t index = m_entity_to_index[entity];
        if (m_request_data.size() != 1){
            uint64_t index_to_move = m_request_data.size() - 1;

            //copy data
            m_request_data[index] = m_request_data[index_to_move];

            //reasing moved entity values
            entt::entity moved_entity = m_index_to_entity[index_to_move];
            m_entity_to_index[moved_entity] = index;
            m_index_to_entity[index] = moved_entity;

            //remove unused data
            m_entity_to_index.erase(entity);
            m_index_to_entity.erase(index_to_move);
            m_request_data.resize(m_request_data.size() -1 );
        }else{
            uint64_t index = m_entity_to_index[entity];
            m_entity_to_index.erase(entity);
            m_index_to_entity.erase(index);
            m_request_data.resize(0);
        }
    }*/
}