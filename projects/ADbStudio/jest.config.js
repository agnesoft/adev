module.exports = {
  preset: '@vue/cli-plugin-unit-jest',
  collectCoverage: true,
    collectCoverageFrom: [
      "src/**/*.{js,vue}",
      "!**/node_modules/**"
    ],
    coverageReporters: [
      "html",
      "text-summary"
    ],
    coverageThreshold: {
      "src/**": {
        branches: 0,
        functions: 0,
        lines: 0,
        statements: 0
      }
    }
}
