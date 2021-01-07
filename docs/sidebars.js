module.exports = {
  someSidebar: {
    "Melon": [
      "introduction",
      {
        "Language": [
          "variables",
          {
            "Value Types": [
              "integer",
              "number",
              "bool",
              "string",
              "array",
              "object",
              "function",
              "range",
              "symbol",
              "iterator",
              "null"
            ]
          },
          "control-structures",
          "operators",
          "modules",
          {
            "Core modules": require("./modules.json")
          }
        ]
      }
    ],
  },
};
