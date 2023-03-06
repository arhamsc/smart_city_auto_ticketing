import { Router } from "express";
import { createBus, getBuses } from "../controllers/bus.controller.js";

const router = Router();

router.route("/").get(getBuses).post(createBus);

export default router;
