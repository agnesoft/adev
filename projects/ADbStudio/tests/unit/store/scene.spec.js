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
})
