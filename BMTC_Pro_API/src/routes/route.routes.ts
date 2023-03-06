import { Router } from "express";
import { createRoute, getRoutes } from "../controllers/route.controller.js";

const router = Router();

router.route("/").get(getRoutes).post(createRoute);

export default router;
