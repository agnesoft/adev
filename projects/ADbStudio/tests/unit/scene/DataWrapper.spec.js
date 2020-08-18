import { mount  } from '@vue/test-utils'
import DataWrapper from '@/components/scene/DataWrapper.vue'

import { localVue,i18n } from '../TestUtils'

describe('DataWrapper',() => {
    let wrapper;
    let fadeTransitionStub; 
    let boxStub; 

    beforeEach(() => {
        
        fadeTransitionStub = () => ({
            props: {
                opened: Boolean,
            },
            template: '<div v-if="opened" ><slot></slot></div>',
          });
        boxStub = () => ({
              props: {
                  opened: Boolean,
              },
              template: '<div class="base-box"> <slot name="title" /><slot name="content"></slot></div>',
            });
        wrapper = mount(DataWrapper, {
            localVue,
            i18n,
            propsData: {
                elements: [
                    { id: 1 },
                    { id: 2, data: {}},
                    { id: 3, data: {foo: "bar"}},
                ]
            },
            stubs: {
                FadeTransition:  fadeTransitionStub(),
                BaseBox: boxStub(),
            },
          });
    })
    it("render 3 boxes", () => {
        expect(wrapper.findAll(".base-box").length).toBe(3);
    });
    it("renders 'no data' in the first two boxes", () => {
        let boxes = wrapper.findAll(".base-box");
        console.log(boxes.at)
        expect(boxes.at(0).find(".no-data").exists()).toBeTruthy();
        expect(boxes.at(0).find(".element-data").exists()).toBeFalsy();
        expect(boxes.at(1).find(".no-data").exists()).toBeTruthy();
        expect(boxes.at(1).find(".element-data").exists()).toBeFalsy();
    });
    it("renders element data in the third box",async () => {
        let boxes = wrapper.findAll(".base-box");
        expect(boxes.at(2).find(".no-data").exists()).toBeFalsy();
        expect(boxes.at(2).find(".element-data").exists()).toBeTruthy();
    });
    it('renders correctly (snapshot)', () => {
        expect(wrapper.element).toMatchSnapshot()
    })
})
