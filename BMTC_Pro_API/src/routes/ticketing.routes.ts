import { Router } from "express";
import {
  boardBusHandler,
  registerCard,
} from "../controllers/ticketing.controller.js";

const router = Router();

router.post("/register-card", registerCard).post("/board", boardBusHandler);

export default router;
