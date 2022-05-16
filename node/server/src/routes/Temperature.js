const express = require("express");
const router = express.Router();

let currentTemperature = 0;

router.post("/", (req, res, next) => {
  currentTemperature = req.body.temperature;
  console.log(req.body);
  res.status(200).send("Temperature updated");
});

router.get("/", (req, res, next) => {
  res.status(200).send({
    temperature: currentTemperature,
  });
});

module.exports = router;
