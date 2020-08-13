import { shallowMount  } from '@vue/test-utils';
import Elements from '@/components/scene/Elements.vue';

import { localVue } from '../TestUtils'


describe('Elements',() => {
    let wrapper
    beforeEach(() => {
        wrapper = shallowMount(Elements, {
            localVue,
            propsData: {
                elements: [{
                    id: 1,
                    coordinates: {
                        x: 0.2,
                        y: 0.5
                    },
                    data: {
                        foo: "bar",
                        foo1: "bar1"
                    }
                },]
            },
        });
    })
    it('emits showData on mouse enter', async () => {
        await wrapper.find(".elements").trigger("mouseenter");
        expect(wrapper.emitted().showData).toBeTruthy();
        expect(wrapper.emitted().showData[0].length).toBe(2);
        expect(wrapper.emitted().showData[0][0][0].data.foo).toBe("bar");
    })
    it('emits showData on mouse leave', async () => {
        await wrapper.find(".elements").trigger("mouseleave");
        expect(wrapper.emitted().showData).toBeTruthy();
        expect(wrapper.emitted().showData.length).toBe(1);
        expect(wrapper.emitted().showData[0][0]).toBe(false);
    })
})
