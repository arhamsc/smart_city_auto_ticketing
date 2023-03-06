//TODO: Register RFID Card
//TODO: Make a boarding when card is scanned

import { NextFunction, Request, Response } from "express";
import { request } from "http";
import { stringify } from "querystring";
import SmartCityDB from "../configs/prisma_client.js";
import ExpressError from "../middlewares/expressError.js";
import { assignBoarding } from "../services/ticketing.service.js";

const prisma = SmartCityDB.getPrismaClient();

export const registerCard = async (
  req: Request,
  res: Response,
  next: NextFunction,
) => {
  try {
    const { body } = req;
    const user_id = body.user_id;

    const user = await prisma.user.findUnique({
      where: {
        id: String(user_id),
      },
    });
    console.log(user);
    var post_data = stringify({
      email: user?.email,
    });
    var post_options = {
      // host: "192.168.1.39",
      host: process.env.ESP_HOST,
      port: "80",
      path: "/write-to-card",
      method: "POST",
      headers: {
        "Content-Type": "application/x-www-form-urlencoded",
        "Content-Length": Buffer.byteLength(post_data),
      },
    };

    var post_req = request(post_options, function (res) {
      res.setEncoding("utf8");
      res.on("data", function (chunk) {
        console.log("Response: " + chunk);
      });
    });

    // post the data
    post_req.write(post_data);
    post_req.end();
    return res.json({ user });
  } catch (error: any) {
    next(new ExpressError({}));
  }
};

export const boardBusHandler = async (
  req: Request,
  res: Response,
  next: NextFunction,
) => {
  try {
    const { body } = req;
    const boarding = await assignBoarding(
      body.user_id,
      body.bus_number,
      body.boarding,
    );
    console.log(body);
    res.json({ boarding });
  } catch (error: any) {
    next(new ExpressError({}));
  }
};
