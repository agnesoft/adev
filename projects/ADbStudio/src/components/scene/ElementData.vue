<template>
    <table  class="element-data">
        <tr v-for="(value, data_key) in data" :key="data_key">
            <td>{{ data_key }}</td>
            <td>{{ value }}</td>
        </tr>
    </table>
</template>

<script>

export default {
    name: "ElementData",
    components: {
    },
    props: {
        data: Object,
    },
    computed: {
    }
}
</script>

<style scoped>
    .element-data{
        width: 100%;
        /* border-radius: 0.4rem; */
        border: solid 1px var(--primary-color);
        padding: 0.4rem;
    }
    .element-data td,.element-data th {
        border: 1px solid #dddddd;
        text-align: left;
        padding: 0.4rem 0.8rem;
    }
    .element-data tr:nth-child(odd) {
        background-color: var(--light-color);
    }
    .element-data tr:nth-child(even) {
        background-color: #dddddd;
    }
    .element-data tr:nth-child(even) td{
        border: 1px solid var(--light-color);
    }
    table.element-data {
        border-collapse: collapse;
        border-style: hidden;
    }

</style>
