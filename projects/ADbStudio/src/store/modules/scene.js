//import axios from 'axios';

const state = {
    nodes: [],
    edges: []
};

const getters = {
    getNodes: state => state.nodes,
    getEdges: state => state.edges
};

const actions = {
    async sendCommand({ commit },{command}) {
        commit('setData',getTestData(command));

        // const response = await axios.get(
        //   'url'+command
        // );
    
        // commit('setData', response.data);
    },
};

const mutations = {
    setData: (state,data) => {
        if(typeof data.nodes !== undefined){
            state.nodes = data.nodes;
        } else {
            state.nodes = [];
        }
        if(typeof data.edges !== undefined){
            state.edges = data.edges;
        } else {
            state.edges = [];
        }
    }
};

export default {
    namespaced: true,
    state: state,
    getters: getters,
    actions: actions,
    mutations: mutations
};


function getTestData(command){
    console.log(command);
    return {
        nodes: [{
                id: 1,
                coordinates: {
                    x: 0.2,
                    y: 0.2,
                }
                
            },{
                id: 2,
                coordinates: {
                    x: 0.3,
                    y: 0.3,
                }
            }],
        edges: [{
            id: -1,
            coordinates1: {
                x: 0.2,
                y: 0.2,
            },
            coordinates2: {
                x: 0.3,
                y: 0.3,
            }
        }],
    }
}
