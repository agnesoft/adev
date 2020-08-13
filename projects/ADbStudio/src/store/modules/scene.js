//import axios from 'axios';

const state = {
    nodes: [],
    edges: [],
    searchError: -1,
};

const getters = {
    getNodes: state => state.nodes,
    getEdges: state => state.edges,
    getEdgesGrouped: state => {
        let grouped = {}
        state.edges.forEach((edge) => {
            if(!Object.prototype.hasOwnProperty.call(edge, 'nodes') || edge.nodes.length < 2){
                return;
            }
            let key = '';
            if(edge.nodes[0] < edge.nodes[1]){
                key = edge.nodes[0]+"_"+edge.nodes[1];
            } else {
                key = edge.nodes[1]+"_"+edge.nodes[0];
            }
            if(Object.prototype.hasOwnProperty.call(grouped, key)){
                grouped[key].push(edge);
            } else {
                grouped[key] = [edge];
            }
        })
        return grouped;
    },
    getData: state => { 
        let data = {};
        if(state.nodes.length > 0){
            data.nodes = state.nodes;
        }
        if(state.edges.length > 0){
            data.edges = state.edges;
        }
        return data
    },
    getSearchError: state => state.searchError,
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
        state.data = data;
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
    },
    search(state,word) {
        state.searchError = 1;
        state.edges.map(edge => edge.selected = false);
        state.nodes.map(node => node.selected = false);
        if(isNaN(word) || word == 0){
            return false;
        }
        if(word < 0){
            let edge = state.edges.find((edge) => edge.id == word);
            if(edge){
                state.searchError = 0;
                edge.selected = true;
                state.searched = word;
            }
        } else if(word > 0) {
            let node = state.nodes.find((node) => node.id == word);
            if(node){
                state.searchError = 0;
                node.selected = true;
                state.searched = word;
            }
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
                },
                data: {
                    name: "Jana",
                    number: 18,
                }
            },{
                id: 4,
                coordinates: {
                    x: 0.6,
                    y: 0.45,
                },
            },{
                id: 1000,
                coordinates: {
                    x: 0.65,
                    y: 0.13,
                },
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
            },
            nodes: [
                1,
                2
            ],
            data: {
                foo: "bar",
                foo1: "bar1",
                foo2: "bar2",
                foo3: "bar2",
                foo4: "bar2",
                foo5: "bar2",
                foo6: "bar2",
                foo7: "bar2",
                foo8: "bar2",
                foo10: "bar2",
                foo11: "bar2",
                foo12: "bar2",
                foo13: "bar2",
                foo14: "bar2",
                foo15: "bar2",
                foo16: "bar2",
                foo17: "bar2",
            },
        },{
            id: -10,
            coordinates1: {
                x: 0.3,
                y: 0.3,
            },
            coordinates2: {
                x: 0.2,
                y: 0.2,
            },
            nodes: [
                2,
                1
            ],
        },{
            id: -450,
            coordinates1: {
                x: 0.6,
                y: 0.45,
            },
            coordinates2: {
                x: 0.65,
                y: 0.13,
            },
            nodes: [
                4,
                1000
            ],
        },{
            id: -1000,
            coordinates1: {
                x: 0.2,
                y: 0.2,
            },
            coordinates2: {
                x: 0.3,
                y: 0.3,
            },
            nodes: [
                1,
                2
            ],
            data: {
                foo: "bar",
                foo1: "bar1",
                foo2: "bar2",
                foo3: "bar2",
                foo4: "bar2",
                foo5: "bar2",
                foo6: "bar2",
                foo7: "bar2",
                foo8: "bar2",
                foo10: "bar2",
                foo11: "bar2",
                foo12: "bar2",
                foo13: "bar2",
                foo14: "bar2",
                foo15: "bar2",
                foo16: "bar2",
                foo17: "bar2",
            },
        },{
            id: -100,
            coordinates1: {
                x: 0.3,
                y: 0.6,
            },
            coordinates2: {
                x: 0.5,
                y: 0.6,
            },
            nodes: [
                15,
                20
            ],
        }],
    }
}
