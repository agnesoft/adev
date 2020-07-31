import { shallowMount  } from '@vue/test-utils';
import Vuex from 'vuex';
import ADbView from '@/components/scene/ADbView.vue';
import Node from '@/components/scene/Node.vue';
import Edge from '@/components/scene/Edge.vue';
import scene_store from '../../../src/store/modules/scene'

import { localVue } from '../TestUtils'


describe('ADbView',() => {
    let wrapper;
    let state;
    let store;

    beforeAll(() => {
        localVue.use(Vuex)
        state = {
            nodes: [{
                id: 1,
                coordinates: {
                    x: 0.2,
                    y: 0.2,
                }
                
            },{
                id: 2,
                coordinates: {
                    x: 0.7,
                    y: 0.7,
                }
            }],
            edges: [{
                id: -1,
                coordinates1: {
                    x: 0.2,
                    y: 0.2,
                },
                coordinates2: {
                    x: 0.7,
                    y: 0.7,
                }
            }],
        }
        
        store = new Vuex.Store({
            modules: {
                scene: {
                    namespaced: true,
                    state,
                    getters: scene_store.getters
                }
              }
        });
        wrapper = shallowMount(ADbView, {
            localVue,
            store
          });
    })
    it('renders nodes and edges',() => {
        expect(wrapper.findComponent(Node).exists()).toBe(true);
        expect(wrapper.findComponent(Edge).exists()).toBe(true);
    })
})
