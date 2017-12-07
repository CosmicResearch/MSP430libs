/*
 * Copyright (c) 2014-2017, Irideon SL. All rights reserved.
 *
 * End-User License Agreement ("Agreement")
 *
 * Please read this End-User License Agreement ("Agreement") carefully before you start
 * using the Application:
 *
 * (a) LICENSE: IRIDEON grants you a revocable, non-exclusive, non-transferable, limited
 * license to download, install and use the Application solely for your personal, non-
 * commercial purposes strictly in accordance with the terms of this Agreement.
 *
 * (b) RESTRICTIONS: You agree not to, and you will not permit others to license, sell,
 * rent, lease, assign, distribute, transmit, host, outsource, disclose or otherwise
 * commercially exploit the Application or make the Application available to any third
 * party.
 *
 * (c) MODIFICATIONS TO APPLICATION: IRIDEON reserves the right to modify, suspend or
 * discontinue, temporarily or permanently, the Application or any service to which it
 * connects, with or without notice and without liability to you.
 *
 * (d) TERM AND TERMINATION: This Agreement shall remain in effect until terminated by
 * you or IRIDEON. IRIDEON may, in its sole discretion, at any time and for any or no
 * reason, suspend or terminate this Agreement with or without prior notice. This
 * Agreement will terminate immediately, without prior notice from IRIDEON, in the event
 * that you fail to comply with any provision of this Agreement. You may also terminate
 * this Agreement by deleting the Application and all copies thereof from your mobile
 * device or from your desktop. Upon termination of this Agreement, you shall cease all
 * use of the Application and delete all copies of the Application from your mobile
 * device or from your desktop.
 *
 * (e) SEVERABILITY: If any provision of this Agreement is held to be unenforceable or
 * invalid, such provision will be changed and interpreted to accomplish the objectives
 * of such provision to the greatest extent possible under applicable law and the
 * remaining provisions will continue in full force and effect.
 *
 * (d) AMENDMENTS TO THIS AGREEMENT: IRIDEON reserves the right, at its sole discretion,
 * to modify or replace this Agreement at any time. If a revision is material we will
 * provide at least 30 (changes this) days' notice prior to any new terms taking effect.
 * What constitutes a material change will be determined at our sole discretion.
 *
 * If you have any questions about this Agreement, please contact us at info@irideon.eu.
 *
 * @author Pancras Villalonga <p.villalonga@irideon.eu>
 * @date   August 18, 2017
 */

#ifndef SENSOR_CLIENT_H_
#define SENSOR_CLIENT_H_

#include "Senscape.h"

struct sensor_data_t {};

class SensorClient {
    public:
        virtual error_t start(void) = 0;
        virtual error_t stop(void) = 0;
        virtual error_t read(void) = 0;
        virtual error_t readNow(void) = 0;
        virtual boolean_t isStarted(void) = 0;

        virtual void attachStartDone(void (*)(error_t)) = 0;
        virtual void attachStopDone(void (*)(error_t)) = 0;
        virtual void attachReadDone(void (*)(sensor_data_t *data, error_t)) = 0;
};

#endif // SENSOR_CLIENT_H_
