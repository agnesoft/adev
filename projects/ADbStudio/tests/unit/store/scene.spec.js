import { localVue } from '../TestUtils'
import Vuex from 'vuex'
import scene from '@/store/modules/scene'
import { cloneDeep } from 'lodash'

describe("Scene Store",() => {
    it("updates nodes and edges when setData is commited",() => {
        localVue.use(Vuex)
        const store = new Vuex.Store(cloneDeep(scene))
        expect(store.state.nodes.length).toBe(0);
        expect(store.state.edges.length).toBe(0);
        store.commit('setData',{
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
        })
        expect(store.state.nodes).toEqual([{
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
        }]);
        expect(store.state.edges).toEqual([{
            id: -1,
            coordinates1: {
                x: 0.2,
                y: 0.2,
            },
            coordinates2: {
                x: 0.3,
                y: 0.3,
            }
        }]);
    })
    it('adds new data to display when newElementData is commited',() => {
        localVue.use(Vuex)
        const store = new Vuex.Store(cloneDeep(scene));
        expect(store.state.elementsData.length).toBe(0);
        store.commit('newElementData',{id: 1, newelement:{
            id: 1,
            coordinates: {
                x: 0.2,
                y: 0.2,
            }
        }});
        console.log(store.state.elementsData[0].id)
        expect(store.state.elementsData).toEqual([{
            id: 1,
            coordinates: {
                x: 0.2,
                y: 0.2,
            }
        },]);

        store.commit('newElementData',{id: 2, newelement:{
            id: 2,
            coordinates: {
                x: 0.3,
                y: 0.3,
            }
        }});
        expect(store.state.elementsData).toEqual([{
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
        }]);

        store.commit('newElementData',{id: 1, newelement:{
            id: 1,
            coordinates: {
                x: 0.2,
                y: 0.2,
            }
        }});
        expect(store.state.elementsData).toEqual([{
            id: 2,
            coordinates: {
                x: 0.3,
                y: 0.3,
            }
        },{
            id: 1,
            coordinates: {
                x: 0.2,
                y: 0.2,
            }
        },]);
    });
    it('removes element to display when removeElementData is called', () => {
        localVue.use(Vuex)
        const store = new Vuex.Store(cloneDeep(scene));
        store.state.elementsData = [{
            id: 2,
            coordinates: {
                x: 0.3,
                y: 0.3,
            }
        },{
            id: 1,
            coordinates: {
                x: 0.2,
                y: 0.2,
            }
        }];
        store.commit('removeElementData',2);
        expect(store.state.elementsData).toEqual([{
            id: 1,
            coordinates: {
                x: 0.2,
                y: 0.2,
            }
        },]);
    });
    it('removes all elements to display when removeAllElementsData is called', () => {
        localVue.use(Vuex)
        const store = new Vuex.Store(cloneDeep(scene));
        store.state.elementsData = [{
            id: 2,
            coordinates: {
                x: 0.3,
                y: 0.3,
            }
        },{
            id: 1,
            coordinates: {
                x: 0.2,
                y: 0.2,
            }
        }];
        store.commit('removeAllElementsData');
        expect(store.state.elementsData.length).toBe(0);
    });
    
})
